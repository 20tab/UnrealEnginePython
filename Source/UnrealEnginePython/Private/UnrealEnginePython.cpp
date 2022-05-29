// Copyright 20Tab S.r.l.

#include "UnrealEnginePython.h"
#include "UEPyModule.h"
#include "PythonBlueprintFunctionLibrary.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformFilemanager.h"
#if !(ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 13))
#include "ClassIconFinder.h"
#endif

#include "Styling/SlateStyleRegistry.h"
#if WITH_EDITOR
#include "IPythonScriptPlugin.h"
#include "Interfaces/IPluginManager.h"
#endif

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 18)
#define PROJECT_CONTENT_DIR FPaths::ProjectContentDir()
#else
#define PROJECT_CONTENT_DIR FPaths::GameContentDir()
#endif

#if PLATFORM_MAC
#include "Runtime/Core/Public/Mac/CocoaThread.h"
#endif

void unreal_engine_init_py_module();
void init_unreal_engine_builtin();

#if PLATFORM_LINUX
// so something seems to have changed between 4.18 and 4.25 which means we can no longer
// import python extension modules that have/are dynamic libraries (ie site?dist-packages modules)
// eg import ctypes fails with missing dynamic link symbol found in the primary python shared library
// it seems that the python extension modules are not able to access the primary
// python shared library /usr/lib/x86_64-linux-gnu/libpythonx.x.so symbols
// OK so this gets even more confusing as apparently on other linux distributions the python extension libraries
// ARE linked with the primary python shared library - this is a debian/ubuntu issue
// it appears what happened is that the default symbol visibility for global symbols was changed from default to hidden
// - likely to reduce chance of symbol clashes - it could be particularly true now with Unreals python implementation
// although that seems to use static linking - but this fixup may mean we cannot have both python implementations
// active
// note that ue4_module_options is a symbol whose existence and value is checked in either LinuxPlatformProcess.cpp (4.18)
// or UnixPlatformProcess.cpp (4.25) to determine if to load dynamic libraries using RTLD_GLOBAL
// - otherwise RTLD_LOCAL is used
// it apparently has to be a global symbol itself for this to work
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
const char *ue4_module_options __attribute__((visibility("default"))) = "linux_global_symbols";
#else
const char *ue4_module_options = "linux_global_symbols";
#endif
#endif

#include "Runtime/Core/Public/Misc/CommandLine.h"
#include "Runtime/Core/Public/Misc/ConfigCacheIni.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformFile.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h"

#include "Runtime/Core/Public/HAL/FileManagerGeneric.h"

#if PLATFORM_WINDOWS
#include <fcntl.h>
#endif

#if PLATFORM_ANDROID
#include "Misc/LocalTimestampDirectoryVisitor.h"
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

const char *UEPyUnicode_AsUTF8(PyObject *py_str)
{
#if PY_MAJOR_VERSION < 3
	if (PyUnicode_Check(py_str))
	{
		PyObject *unicode = PyUnicode_AsUTF8String(py_str);
		if (unicode)
		{
			return PyString_AsString(unicode);
		}
		// just a hack to avoid crashes
		return (char *)"<invalid_string>";
	}
	return (const char *)PyString_AsString(py_str);
#elif PY_MINOR_VERSION < 7
	return (const char *)PyUnicode_AsUTF8(py_str);
#else
	return PyUnicode_AsUTF8(py_str);
#endif
}

#if PY_MAJOR_VERSION < 3
int PyGILState_Check()
{
	PyThreadState * tstate = _PyThreadState_Current;
	return tstate && (tstate == PyGILState_GetThisThreadState());
}
#endif

bool PyUnicodeOrString_Check(PyObject *py_obj)
{
	if (PyUnicode_Check(py_obj))
	{
		return true;
	}
#if PY_MAJOR_VERSION < 3
	else if (PyString_Check(py_obj))
	{
		return true;
	}
#endif
	return false;
}

#define LOCTEXT_NAMESPACE "FUnrealEnginePythonModule"

void FUnrealEnginePythonModule::UESetupPythonInterpreter(bool verbose)
{
	const TCHAR* CommandLine = FCommandLine::GetOriginal();
	const SIZE_T CommandLineSize = FCString::Strlen(CommandLine) + 1;
	TCHAR* CommandLineCopy = new TCHAR[CommandLineSize];
	FCString::Strcpy(CommandLineCopy, CommandLineSize, CommandLine);
	const TCHAR* ParsedCmdLine = CommandLineCopy;

	TArray<FString> Args;
	for (;;)
	{
		FString Arg = FParse::Token(ParsedCmdLine, 0);
		if (Arg.Len() <= 0)
			break;
		Args.Add(Arg);
	}

#if PY_MAJOR_VERSION >= 3
	wchar_t **argv = (wchar_t **)FMemory::Malloc(sizeof(wchar_t *) * (Args.Num() + 1));
#else
	char **argv = (char **)FMemory::Malloc(sizeof(char *) * (Args.Num() + 1));
#endif
	argv[Args.Num()] = nullptr;

	for (int32 i = 0; i < Args.Num(); i++)
	{
#if PY_MAJOR_VERSION >= 3
	#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20)
		argv[i] = (wchar_t *)(TCHAR_TO_WCHAR(*Args[i]));
	#else
		argv[i] = (wchar_t *)(*Args[i]);
	#endif
#else
		argv[i] = TCHAR_TO_UTF8(*Args[i]);
#endif
	}

	PySys_SetArgv(Args.Num(), argv);

	unreal_engine_init_py_module();

	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_path = PyDict_GetItemString(py_sys_dict, "path");

	char *zip_path = TCHAR_TO_UTF8(*ZipPath);
	PyObject *py_zip_path = PyUnicode_FromString(zip_path);
	PyList_Insert(py_path, 0, py_zip_path);


	int i = 0;
	for (FString ScriptsPath : ScriptsPaths)
	{
		char *scripts_path = TCHAR_TO_UTF8(*ScriptsPath);
		PyObject *py_scripts_path = PyUnicode_FromString(scripts_path);
		PyList_Insert(py_path, i++, py_scripts_path);
		if (verbose)
		{
			UE_LOG(LogPython, Log, TEXT("Python Scripts search path: %s"), UTF8_TO_TCHAR(scripts_path));
		}
	}

	char *additional_modules_path = TCHAR_TO_UTF8(*AdditionalModulesPath);
	PyObject *py_additional_modules_path = PyUnicode_FromString(additional_modules_path);
	PyList_Insert(py_path, 0, py_additional_modules_path);

	if (verbose)
	{
		UE_LOG(LogPython, Log, TEXT("Python VM initialized: %s"), UTF8_TO_TCHAR(Py_GetVersion()));
	}
}

static void setup_stdout_stderr()
{
	// Redirecting stdout
	char const* code = "import sys\n"
		"import unreal_engine\n"
		"class UnrealEngineOutput:\n"
		"    def __init__(self, logger):\n"
		"        self.logger = logger\n"
		"    def write(self, buf):\n"
		"        self.logger(buf)\n"
		"    def flush(self):\n"
		"        return\n"
		"    def isatty(self):\n"
		"        return False\n"
		"sys.stdout = UnrealEngineOutput(unreal_engine.log)\n"
		"sys.stderr = UnrealEngineOutput(unreal_engine.log_error)\n"
		"\n"
		"class event:\n"
		"    def __init__(self, event_signature):\n"
		"        self.event_signature = event_signature\n"
		"    def __call__(self, f):\n"
		"        f.ue_event = self.event_signature\n"
		"        return f\n"
		"\n"
		"unreal_engine.event = event";
	PyRun_SimpleString(code);
}

namespace
{
	static void consoleExecScript(const TArray<FString>& Args)
	{
		if (Args.Num() != 1)
		{
			UE_LOG(LogPython, Warning, TEXT("Usage: 'py.exec <scriptname>'."));
			UE_LOG(LogPython, Warning, TEXT("  scriptname: Name of script, must reside in Scripts folder. Ex: myscript.py"));
		}
		else
		{
			UPythonBlueprintFunctionLibrary::ExecutePythonScript(Args[0]);
		}
	}

	static void consoleExecString(const TArray<FString>& Args)
	{
		if (Args.Num() == 0)
		{
			UE_LOG(LogPython, Warning, TEXT("Usage: 'py.cmd <command string>'."));
			UE_LOG(LogPython, Warning, TEXT("  scriptname: Name of script, must reside in Scripts folder. Ex: myscript.py"));
		}
		else
		{
			FString cmdString;
			for (const FString& argStr : Args)
			{
				cmdString += argStr.TrimQuotes() + '\n';
			}
			UPythonBlueprintFunctionLibrary::ExecutePythonString(cmdString);
		}
	}

}
FAutoConsoleCommand ExecPythonScriptCommand(
	TEXT("py.exec"),
	*NSLOCTEXT("UnrealEnginePython", "CommandText_Exec", "Execute python script").ToString(),
	FConsoleCommandWithArgsDelegate::CreateStatic(consoleExecScript));

FAutoConsoleCommand ExecPythonStringCommand(
	TEXT("py.cmd"),
	*NSLOCTEXT("UnrealEnginePython", "CommandText_Cmd", "Execute python string").ToString(),
	FConsoleCommandWithArgsDelegate::CreateStatic(consoleExecString));


void FUnrealEnginePythonModule::StartupModule()
{
#if WITH_EDITOR
	if (Py_IsInitialized())
	{
		IPythonScriptPlugin::Get()->ShutdownModule();
	}
#endif

	BrutalFinalize = false;

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 7
	// Python 3.7+ changes the C locale which affects functions using C string APIs
	// So change the C locale back to its current setting after Py_Initialize has been called
	FString CurrentLocale;

	if (const char* CurrentLocalePtr = setlocale(LC_ALL, nullptr))
	{
		CurrentLocale = ANSI_TO_TCHAR(CurrentLocalePtr);
	}
#endif

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PythonHome;
	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("Home"), PythonHome, GEngineIni))
	{
#if PY_MAJOR_VERSION >= 3
	#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20)
		wchar_t *home = (wchar_t *)(TCHAR_TO_WCHAR(*PythonHome));
	#else
		wchar_t *home = (wchar_t *)*PythonHome;
	#endif
#else
		char *home = TCHAR_TO_UTF8(*PythonHome);
#endif
		FPlatformMisc::SetEnvironmentVar(TEXT("PYTHONHOME"), *PythonHome);
		Py_SetPythonHome(home);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeHome"), PythonHome, GEngineIni))
	{
		PythonHome = FPaths::Combine(*PROJECT_CONTENT_DIR, *PythonHome);
		FPaths::NormalizeFilename(PythonHome);
		PythonHome = FPaths::ConvertRelativePathToFull(PythonHome);
#if PY_MAJOR_VERSION >= 3
	#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20)
		wchar_t *home = (wchar_t *)(TCHAR_TO_WCHAR(*PythonHome));
	#else
		wchar_t *home = (wchar_t *)*PythonHome;
	#endif
#else
		char *home = TCHAR_TO_UTF8(*PythonHome);
#endif

		Py_SetPythonHome(home);
	}

	TArray<FString> ImportModules;

	FString IniValue;
	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ProgramName"), IniValue, GEngineIni))
	{
#if PY_MAJOR_VERSION >= 3
	#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20)
		wchar_t *program_name = (wchar_t *)(TCHAR_TO_WCHAR(*IniValue));
	#else
		wchar_t *program_name = (wchar_t *)*IniValue;
	#endif
#else
		char *program_name = TCHAR_TO_UTF8(*IniValue);
#endif
		Py_SetProgramName(program_name);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeProgramName"), IniValue, GEngineIni))
	{
		IniValue = FPaths::Combine(*PROJECT_CONTENT_DIR, *IniValue);
		FPaths::NormalizeFilename(IniValue);
		IniValue = FPaths::ConvertRelativePathToFull(IniValue);
#if PY_MAJOR_VERSION >= 3
	#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 20)
		wchar_t *program_name = (wchar_t *)(TCHAR_TO_WCHAR(*IniValue));
	#else
		wchar_t *program_name = (wchar_t *)*IniValue;
	#endif
#else
		char *program_name = TCHAR_TO_UTF8(*IniValue);
#endif
		Py_SetProgramName(program_name);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ScriptsPath"), IniValue, GEngineIni))
	{
		ScriptsPaths.Add(IniValue);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeScriptsPath"), IniValue, GEngineIni))
	{
		ScriptsPaths.Add(FPaths::Combine(*PROJECT_CONTENT_DIR, *IniValue));
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("AdditionalModulesPath"), IniValue, GEngineIni))
	{
		AdditionalModulesPath = IniValue;
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeAdditionalModulesPath"), IniValue, GEngineIni))
	{
		AdditionalModulesPath = FPaths::Combine(*PROJECT_CONTENT_DIR, *IniValue);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ZipPath"), IniValue, GEngineIni))
	{
		ZipPath = IniValue;
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeZipPath"), IniValue, GEngineIni))
	{
		ZipPath = FPaths::Combine(*PROJECT_CONTENT_DIR, *IniValue);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ImportModules"), IniValue, GEngineIni))
	{
		const TCHAR* separators[] = { TEXT(" "), TEXT(";"), TEXT(",") };
		IniValue.ParseIntoArray(ImportModules, separators, 3);
	}

	FString ProjectScriptsPath = FPaths::Combine(*PROJECT_CONTENT_DIR, UTF8_TO_TCHAR("Scripts"));
	if (!FPaths::DirectoryExists(ProjectScriptsPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectory(*ProjectScriptsPath);
	}
	ScriptsPaths.Add(ProjectScriptsPath);

#if WITH_EDITOR
	for (TSharedRef<IPlugin>plugin : IPluginManager::Get().GetEnabledPlugins())
	{
		FString PluginScriptsPath = FPaths::Combine(plugin->GetContentDir(), UTF8_TO_TCHAR("Scripts"));
		if (FPaths::DirectoryExists(PluginScriptsPath))
		{
			ScriptsPaths.Add(PluginScriptsPath);
		}

		// allows third parties to include their code in the main plugin directory
		if (plugin->GetName() == "UnrealEnginePython")
		{
			ScriptsPaths.Add(plugin->GetBaseDir());
		}
	}

#if PLATFORM_WINDOWS
	ScriptsPaths.Add(FPaths::Combine(FPaths::EngineDir(), "Binaries", "ThirdParty", "Python3", "Win64", "Lib",
	                                 "site-packages"));
#endif
#endif

	if (ZipPath.IsEmpty())
	{
		ZipPath = FPaths::Combine(*PROJECT_CONTENT_DIR, UTF8_TO_TCHAR("ue_python.zip"));
	}

	// To ensure there are no path conflicts, if we have a valid python home at this point,
	// we override the current environment entirely with the environment we want to use,
	// removing any paths to other python environments we aren't using.
	if (PythonHome.Len() > 0)
	{
		FPlatformMisc::SetEnvironmentVar(TEXT("PYTHONHOME"), *PythonHome);

		const int32 MaxPathVarLen = 32768;
		FString OrigPathVar = FString::ChrN(MaxPathVarLen, TEXT('\0'));
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 21)
		OrigPathVar = FPlatformMisc::GetEnvironmentVariable(TEXT("PATH"));
#else
		FPlatformMisc::GetEnvironmentVariable(TEXT("PATH"), OrigPathVar.GetCharArray().GetData(), MaxPathVarLen);
#endif

		// Get the current path and remove elements with python in them, we don't want any conflicts
		const TCHAR* PathDelimiter = FPlatformMisc::GetPathVarDelimiter();
		TArray<FString> PathVars;
		OrigPathVar.ParseIntoArray(PathVars, PathDelimiter, true);
		for (int32 PathRemoveIndex = PathVars.Num() - 1; PathRemoveIndex >= 0; --PathRemoveIndex)
		{
			if (PathVars[PathRemoveIndex].Contains(TEXT("python"), ESearchCase::IgnoreCase))
			{
				UE_LOG(LogPython, Verbose, TEXT("Removing other python Path: '%s'"), *PathVars[PathRemoveIndex]);
				PathVars.RemoveAt(PathRemoveIndex);
			}
		}

		// Setup our own paths for PYTHONPATH
		#if PLATFORM_WINDOWS
		TArray<FString> OurPythonPaths = {
			PythonHome,
			FPaths::Combine(PythonHome, TEXT("Lib")),
			FPaths::Combine(PythonHome, TEXT("Lib/site-packages")),
		};
		FString PythonPathVars = FString::Join(OurPythonPaths, PathDelimiter);
		FPlatformMisc::SetEnvironmentVar(TEXT("PYTHONPATH"), *PythonPathVars);

		// Also add our paths to PATH, just so any searching will find our local python
		PathVars.Append(OurPythonPaths);
		FString ModifiedPath = FString::Join(PathVars, PathDelimiter);
		FPlatformMisc::SetEnvironmentVar(TEXT("PATH"), *ModifiedPath);
		#endif
	}


#if PY_MAJOR_VERSION >= 3
	init_unreal_engine_builtin();
#if PLATFORM_ANDROID
	FString InDirectory = FString(TEXT("Scripts"));

	extern FString GExternalFilePath;

	FString DirectoryPath = FPaths::ProjectContentDir() / InDirectory;

	IFileManager* FileManager = &IFileManager::Get();

	// iterate over all the files in provided directory
	FLocalTimestampDirectoryVisitor Visitor(FPlatformFileManager::Get().GetPlatformFile(), TArray<FString>(),
	                                        TArray<FString>(), false);

	FileManager->IterateDirectoryRecursively(*DirectoryPath, Visitor);

	FString Prefix = FApp::GetProjectName() / FString(TEXT("Content/Scripts/"));

	for (TMap<FString, FDateTime>::TIterator TimestampIt(Visitor.FileTimes); TimestampIt; ++TimestampIt)
	{
		FString Path = TimestampIt.Key();

		Path.RemoveFromStart(Prefix);

		// read the file contents and write it if successful to external path
		TArray<uint8> MemFile;

		const FString SourceFilename = TimestampIt.Key();

		if (FFileHelper::LoadFileToArray(MemFile, *SourceFilename, 0))
		{
			FString DestFilename = GExternalFilePath / InDirectory / Path;

			FFileHelper::SaveArrayToFile(MemFile, *DestFilename);
		}
	}

	FString PyScriptsSearchPath = GExternalFilePath / InDirectory;

	ScriptsPaths.Reset();

	ScriptsPaths.Add(PyScriptsSearchPath);

	UE_LOG(LogPython, Warning, TEXT("Setting Android Python Scripts Search Path to %s"), *PyScriptsSearchPath);

	Py_SetPath(Py_DecodeLocale(TCHAR_TO_UTF8(*PyScriptsSearchPath), NULL));
#elif PLATFORM_IOS
    FString IOSContentPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*IFileManager::Get().GetFilenameOnDisk(*FPaths::ConvertRelativePathToFull(PROJECT_CONTENT_DIR)));
    FString PyScriptsSearchPath = IOSContentPath / FString(TEXT("lib")) + FString(":") +
                                IOSContentPath / FString(TEXT("lib/stdlib.zip")) + FString(":") +
                                IOSContentPath / FString(TEXT("scripts")); // the name of directory must be lower-case.

    Py_SetPath(Py_DecodeLocale(TCHAR_TO_UTF8(*PyScriptsSearchPath), NULL));
    
    UE_LOG(LogPython, Log, TEXT("Setting IOS Python Scripts Search Path to %s"), *PyScriptsSearchPath);
#endif
#endif

#if PLATFORM_IOS || PLATFORM_ANDROID
    Py_NoSiteFlag = 1;
    Py_NoUserSiteDirectory = 1;
    
    Py_InitializeEx(0);
#else
	Py_Initialize();
#endif

#if PLATFORM_WINDOWS
	// Restore stdio state after Py_Initialize set it to O_BINARY, otherwise
	// everything that the engine will output is going to be encoded in UTF-16.
	// The behaviour is described here: https://bugs.python.org/issue16587
	_setmode(_fileno(stdin), O_TEXT);
	_setmode(_fileno(stdout), O_TEXT);
	_setmode(_fileno(stderr), O_TEXT);

	// Also restore the user-requested UTF-8 flag if relevant (behaviour copied
	// from LaunchEngineLoop.cpp).
	if (FParse::Param(FCommandLine::Get(), TEXT("UTF8Output")))
	{
		FPlatformMisc::SetUTF8Output();
	}
#endif

	PyEval_InitThreads();

#if WITH_EDITOR
	StyleSet = MakeShareable(new FSlateStyleSet("UnrealEnginePython"));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("UnrealEnginePython")->GetBaseDir() / "Resources");
	StyleSet->Set("ClassThumbnail.PythonScript", new FSlateImageBrush(StyleSet->RootToContentDir("Icon128.png"), FVector2D(128.0f, 128.0f)));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
#if !(ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 13))
	FClassIconFinder::RegisterIconSource(StyleSet.Get());
#endif
#endif

#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 7
	// We call setlocale here to restore the previous state
	if (!CurrentLocale.IsEmpty())
	{
		setlocale(LC_ALL, TCHAR_TO_ANSI(*CurrentLocale));
	}
#endif

	UESetupPythonInterpreter(true);

	main_module = PyImport_AddModule("__main__");
	main_dict = PyModule_GetDict((PyObject*)main_module);
	local_dict = main_dict;// PyDict_New();

	setup_stdout_stderr();

	if (PyImport_ImportModule("ue_site"))
	{
		UE_LOG(LogPython, Log, TEXT("ue_site Python module successfully imported"));
	}
	else
	{
#if PY_MAJOR_VERSION >= 3 && PY_MINOR_VERSION >= 6
		if (PyErr_ExceptionMatches(PyExc_ModuleNotFoundError))
		{
			UE_LOG(LogPython, Log, TEXT("ue_site Python module not found"));
			PyErr_Clear();
		}
		else
		{
			unreal_engine_py_log_error();
		}
#else
		unreal_engine_py_log_error();
#endif
	}


	for (FString ImportModule : ImportModules)
	{
		if (PyImport_ImportModule(TCHAR_TO_UTF8(*ImportModule)))
		{
			UE_LOG(LogPython, Log, TEXT("%s Python module successfully imported"), *ImportModule);
		}
		else
		{
			unreal_engine_py_log_error();
		}
	}

	// release the GIL
	PyThreadState *UEPyGlobalState = PyEval_SaveThread();
}

void FUnrealEnginePythonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogPython, Log, TEXT("Goodbye Python"));
	if (!BrutalFinalize)
	{
		PyGILState_Ensure();
		Py_Finalize();
	}
}

void FUnrealEnginePythonModule::RunString(char *str)
{
	FScopePythonGIL gil;

	PyObject *eval_ret = PyRun_String(str, Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	if (!eval_ret)
	{
		if (PyErr_ExceptionMatches(PyExc_SystemExit))
		{
			PyErr_Clear();
			return;
		}
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(eval_ret);
}

#if PLATFORM_MAC
void FUnrealEnginePythonModule::RunStringInMainThread(char *str)
{
	MainThreadCall(^{
	RunString(str);
		});
}

void FUnrealEnginePythonModule::RunFileInMainThread(char *filename)
{
	MainThreadCall(^{
	RunFile(filename);
		});
}
#endif

FString FUnrealEnginePythonModule::Pep8ize(FString Code)
{
	FScopePythonGIL gil;

	PyObject *pep8izer_module = PyImport_ImportModule("autopep8");
	if (!pep8izer_module)
	{
		unreal_engine_py_log_error();
		UE_LOG(LogPython, Error, TEXT("unable to load autopep8 module, please install it"));
		// return the original string to avoid losing data
		return Code;
	}

	PyObject *pep8izer_func = PyObject_GetAttrString(pep8izer_module, (char*)"fix_code");
	if (!pep8izer_func)
	{
		unreal_engine_py_log_error();
		UE_LOG(LogPython, Error, TEXT("unable to get autopep8.fix_code function"));
		// return the original string to avoid losing data
		return Code;
	}

	PyObject *ret = PyObject_CallFunction(pep8izer_func, (char *)"s", TCHAR_TO_UTF8(*Code));
	if (!ret)
	{
		unreal_engine_py_log_error();
		// return the original string to avoid losing data
		return Code;
	}

	if (!PyUnicodeOrString_Check(ret))
	{
		UE_LOG(LogPython, Error, TEXT("returned value is not a string"));
		// return the original string to avoid losing data
		return Code;
	}

	const char *pep8ized = UEPyUnicode_AsUTF8(ret);
	FString NewCode = FString(pep8ized);
	Py_DECREF(ret);

	return NewCode;
}


void FUnrealEnginePythonModule::RunFile(char *filename)
{
	FScopePythonGIL gil;
	FString full_path = UTF8_TO_TCHAR(filename);
	FString original_path = full_path;
	bool foundFile = false;
	if (!FPaths::FileExists(filename))
	{
		for (FString ScriptsPath : ScriptsPaths)
		{
			full_path = FPaths::Combine(*ScriptsPath, original_path);

#if PLATFORM_IOS
			full_path = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*full_path);
#endif
			
			if (FPaths::FileExists(full_path))
			{
				foundFile = true;
				break;
			}
		}
	}
	else
	{
		foundFile = true;
	}

	if (!foundFile)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to find file %s"), UTF8_TO_TCHAR(filename));
#if !PLATFORM_IOS
		return;
#endif
	}

#if PY_MAJOR_VERSION >= 3
	FILE *fd = nullptr;

#if PLATFORM_WINDOWS
	if (fopen_s(&fd, TCHAR_TO_UTF8(*full_path), "r") != 0)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), *full_path);
		return;
	}
#else
	fd = fopen(TCHAR_TO_UTF8(*full_path), "r");
	if (!fd)
	{
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), *full_path);
		return;
	}
#endif

	PyObject *eval_ret = PyRun_File(fd, TCHAR_TO_UTF8(*full_path), Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	fclose(fd);
	if (!eval_ret)
	{
		if (PyErr_ExceptionMatches(PyExc_SystemExit))
		{
			PyErr_Clear();
			return;
		}
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(eval_ret);
#else
	// damn, this is horrible, but it is the only way i found to avoid the CRT error :(
	FString command = FString::Printf(TEXT("execfile(\"%s\")"), *full_path);
	PyObject *eval_ret = PyRun_String(TCHAR_TO_UTF8(*command), Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	if (!eval_ret)
	{
		if (PyErr_ExceptionMatches(PyExc_SystemExit))
		{
			PyErr_Clear();
			return;
		}
		unreal_engine_py_log_error();
		return;
	}
#endif

}


void ue_py_register_magic_module(char *name, PyObject *(*func)())
{
	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_sys_modules = PyDict_GetItemString(py_sys_dict, "modules");
	PyObject *u_module = func();
	Py_INCREF(u_module);
	PyDict_SetItemString(py_sys_modules, name, u_module);
}

PyObject *ue_py_register_module(const char *name)
{
	return PyImport_AddModule(name);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePythonModule, UnrealEnginePython)
