// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealEnginePythonPrivatePCH.h"

void unreal_engine_init_py_module();

#if defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
const char *ue4_module_options = "linux_global_symbols";
#endif

#if PY_MAJOR_VERSION < 3
char *PyUnicode_AsUTF8(PyObject *py_str) {
	if (PyUnicode_Check(py_str)) {
		PyObject *unicode = PyUnicode_AsUTF8String(py_str);
		if (unicode) {
			return PyString_AsString(unicode);
		}
		// just a hack to avoid crashes
		return (char *)"<invalid_string>";
	}
	return PyString_AsString(py_str);
}

int PyGILState_Check() {
	PyThreadState * tstate = _PyThreadState_Current;
	return tstate && (tstate == PyGILState_GetThisThreadState());
}
#endif

bool PyUnicodeOrString_Check(PyObject *py_obj) {
	if (PyUnicode_Check(py_obj)) {
		return true;
	}
#if PY_MAJOR_VERSION < 3
	else if (PyString_Check(py_obj)) {
		return true;
	}
#endif
	return false;
}

#define LOCTEXT_NAMESPACE "FUnrealEnginePythonModule"


void FUnrealEnginePythonModule::PythonGILRelease() {
#if defined(UEPY_THREADING)
	if (PyGILState_Check() == 1) {
		ue_python_gil = PyEval_SaveThread();
	}
#endif
}

bool FUnrealEnginePythonModule::PythonGILAcquire() {
#if defined(UEPY_THREADING)
	if (PyGILState_Check() == 0) {
		PyEval_RestoreThread((PyThreadState *)ue_python_gil);
		return true;
	}
	return false;
#endif
	return true;
}

void FUnrealEnginePythonModule::UESetupPythonInterpreter(bool verbose) {

#if PY_MAJOR_VERSION >= 3
	wchar_t *argv[] = { UTF8_TO_TCHAR("UnrealEngine"), NULL };
#else
	char *argv[] = { (char *)"UnrealEngine", NULL };
#endif
	PySys_SetArgv(1, argv);

	unreal_engine_init_py_module();

	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_path = PyDict_GetItemString(py_sys_dict, "path");

	char *zip_path = TCHAR_TO_UTF8(*ZipPath);
	PyObject *py_zip_path = PyUnicode_FromString(zip_path);
	PyList_Insert(py_path, 0, py_zip_path);

	char *scripts_path = TCHAR_TO_UTF8(*ScriptsPath);
	PyObject *py_scripts_path = PyUnicode_FromString(scripts_path);
	PyList_Insert(py_path, 0, py_scripts_path);

	if (verbose) {
		UE_LOG(LogPython, Log, TEXT("Python VM initialized: %s"), UTF8_TO_TCHAR(Py_GetVersion()));
		UE_LOG(LogPython, Log, TEXT("Python Scripts search path: %s"), UTF8_TO_TCHAR(scripts_path));
	}
}

static void setup_stdout_stderr() {
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
		"sys.stdout = UnrealEngineOutput(unreal_engine.log)\n"
		"sys.stderr = UnrealEngineOutput(unreal_engine.log_error)\n";
	PyRun_SimpleString(code);
}

void FUnrealEnginePythonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString IniValue;
	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("Home"), IniValue, GEngineIni)) {
#if PY_MAJOR_VERSION >= 3
		wchar_t *home = (wchar_t *)*IniValue;
#else
		char *home = TCHAR_TO_UTF8(*IniValue);
#endif

		Py_SetPythonHome(home);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ScriptsPath"), IniValue, GEngineIni)) {
		ScriptsPath = IniValue;
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeScriptsPath"), IniValue, GEngineIni)) {
		ScriptsPath = FPaths::Combine(FPaths::GameContentDir(), IniValue);
	}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("ZipPath"), IniValue, GEngineIni)) {
		ZipPath = IniValue;
}

	if (GConfig->GetString(UTF8_TO_TCHAR("Python"), UTF8_TO_TCHAR("RelativeZipPath"), IniValue, GEngineIni)) {
		ZipPath = FPaths::Combine(FPaths::GameContentDir(), IniValue);
	}

	if (ScriptsPath.IsEmpty()) {
		ScriptsPath = FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("Scripts"));
	}

	if (ZipPath.IsEmpty()) {
		ZipPath = FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("ue_python.zip"));
	}

	Py_Initialize();

	PyEval_InitThreads();

#if WITH_EDITOR
	StyleSet = MakeShareable(new FSlateStyleSet("UnrealEnginePython"));
	StyleSet->SetContentRoot(IPluginManager::Get().FindPlugin("UnrealEnginePython")->GetBaseDir() / "Resources");
	StyleSet->Set("ClassThumbnail.PythonScript", new FSlateImageBrush(StyleSet->RootToContentDir("Icon128.png"), FVector2D(128.0f, 128.0f)));
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
#if ENGINE_MINOR_VERSION < 13
	FClassIconFinder::RegisterIconSource(StyleSet.Get());
#endif
#endif

	UESetupPythonInterpreter(true);

	main_module = PyImport_AddModule("__main__");
	main_dict = PyModule_GetDict((PyObject*)main_module);
	local_dict = main_dict;// PyDict_New();

	setup_stdout_stderr();

	if (PyImport_ImportModule("ue_site")) {
		UE_LOG(LogPython, Log, TEXT("ue_site Python module successfully imported"));
	}
	else {
		// TODO gracefully manage the error
		unreal_engine_py_log_error();
	}

	// release the GIL
	PythonGILRelease();

}

void FUnrealEnginePythonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogPython, Log, TEXT("Goodbye Python"));
	PythonGILAcquire();
	Py_Finalize();
}

void FUnrealEnginePythonModule::RunString(char *str) {
	FScopePythonGIL gil;
	PyObject *eval_ret = PyRun_String(str, Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(eval_ret);
}

// run a python string in a new sub interpreter 
void FUnrealEnginePythonModule::RunStringSandboxed(char *str) {
	FScopePythonGIL gil;

	PyThreadState *_main = PyThreadState_Get();

	PyThreadState *py_new_state = Py_NewInterpreter();
	if (!py_new_state) {
		UE_LOG(LogPython, Error, TEXT("Unable to create new Python interpreter"));
		return;
	}
	PyThreadState_Swap(nullptr);
	PyThreadState_Swap(py_new_state);

	UESetupPythonInterpreter(false);

	setup_stdout_stderr();

	PyObject *m = PyImport_AddModule("__main__");
	if (m == NULL) {
		UE_LOG(LogPython, Error, TEXT("Unable to create new global dict"));
		Py_EndInterpreter(py_new_state);
		PyThreadState_Swap(_main);
		return;
	}
	PyObject *global_dict = PyModule_GetDict(m);

	PyObject *eval_ret = PyRun_String(str, Py_file_input, global_dict, global_dict);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		Py_EndInterpreter(py_new_state);
		PyThreadState_Swap(_main);
		return;
	}

	Py_EndInterpreter(py_new_state);
	PyThreadState_Swap(_main);
	}

void FUnrealEnginePythonModule::RunFile(char *filename) {
	FScopePythonGIL gil;
	char *full_path = filename;
	if (!FPaths::FileExists(filename))
	{
		full_path = TCHAR_TO_UTF8(*FPaths::Combine(ScriptsPath, UTF8_TO_TCHAR(filename)));
	}
#if PY_MAJOR_VERSION >= 3
	FILE *fd = nullptr;

#if PLATFORM_WINDOWS
	if (fopen_s(&fd, full_path, "r") != 0) {
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), UTF8_TO_TCHAR(full_path));
		return;
	}
#else
	fd = fopen(full_path, "r");
	if (!fd) {
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), UTF8_TO_TCHAR(full_path));
		return;
	}
#endif

	PyObject *eval_ret = PyRun_File(fd, full_path, Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	fclose(fd);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(eval_ret);
#else
	// damn, this is horrible, but it is the only way i found to avoid the CRT error :(
	FString command = FString::Printf(TEXT("execfile(\"%s\")"), UTF8_TO_TCHAR(full_path));
	PyObject *eval_ret = PyRun_String(TCHAR_TO_UTF8(*command), Py_file_input, (PyObject *)main_dict, (PyObject *)local_dict);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		return;
	}
#endif

}

// run a python script in a new sub interpreter (useful for unit tests)
void FUnrealEnginePythonModule::RunFileSandboxed(char *filename) {
	FScopePythonGIL gil;
	char *full_path = filename;
	if (!FPaths::FileExists(filename))
	{
		full_path = TCHAR_TO_UTF8(*FPaths::Combine(ScriptsPath, UTF8_TO_TCHAR(filename)));
	}

	PyThreadState *_main = PyThreadState_Get();

	PyThreadState *py_new_state = Py_NewInterpreter();
	if (!py_new_state) {
		UE_LOG(LogPython, Error, TEXT("Unable to create new Python interpreter"));
		return;
	}
	PyThreadState_Swap(nullptr);
	PyThreadState_Swap(py_new_state);

	UESetupPythonInterpreter(false);

	setup_stdout_stderr();

	PyObject *m = PyImport_AddModule("__main__");
	if (m == NULL) {
		UE_LOG(LogPython, Error, TEXT("Unable to create new global dict"));
		Py_EndInterpreter(py_new_state);
		PyThreadState_Swap(_main);
		return;
	}
	PyObject *global_dict = PyModule_GetDict(m);

#if PY_MAJOR_VERSION >= 3
	FILE *fd = nullptr;

#if PLATFORM_WINDOWS
	if (fopen_s(&fd, full_path, "r") != 0) {
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), UTF8_TO_TCHAR(full_path));
		return;
	}
#else
	fd = fopen(full_path, "r");
	if (!fd) {
		UE_LOG(LogPython, Error, TEXT("Unable to open file %s"), UTF8_TO_TCHAR(full_path));
		return;
	}
#endif
	PyObject *eval_ret = PyRun_File(fd, full_path, Py_file_input, global_dict, global_dict);
	fclose(fd);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		Py_EndInterpreter(py_new_state);
		PyThreadState_Swap(_main);
		return;
	}
	Py_DECREF(eval_ret);
#else
	// damn, this is horrible, but it is the only way i found to avoid the CRT error :(
	FString command = FString::Printf(TEXT("execfile(\"%s\")"), UTF8_TO_TCHAR(full_path));
	PyObject *eval_ret = PyRun_String(TCHAR_TO_UTF8(*command), Py_file_input, global_dict, global_dict);
	if (!eval_ret) {
		unreal_engine_py_log_error();
		Py_EndInterpreter(py_new_state);
		PyThreadState_Swap(_main);
		return;
	}
#endif
	Py_EndInterpreter(py_new_state);
	PyThreadState_Swap(_main);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePythonModule, UnrealEnginePython)

