#include "UnrealEnginePythonPrivatePCH.h"
#include "PyCommandlet.h"

#include "Regex.h"

UPyCommandlet::UPyCommandlet(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	LogToConsole = 1;
}

int32 UPyCommandlet::Main(const FString& CommandLine)
{
	TArray<FString> Tokens, Switches;
	TMap<FString, FString> Params;
	ParseCommandLine(*CommandLine, Tokens, Switches, Params);

	FString Filepath = Tokens[0];
	if (!FPaths::FileExists(*Filepath))
	{
		UE_LOG(LogPython, Error, TEXT("Python file could not be found: %s"), *Filepath);
		return -1;
	}

	FString RegexString = FString::Printf(TEXT("(?<=%s).*"), *(Filepath.Replace(TEXT("\\"), TEXT("\\\\"))));
	const FRegexPattern myPattern(RegexString);
	FRegexMatcher myMatcher(myPattern, *CommandLine);
	myMatcher.FindNext();
	FString PyCommandLine = myMatcher.GetCaptureGroup(0).Trim().TrimTrailing();
	
	TArray<FString> PyArgv;
	PyArgv.Add(FString());
	bool escaped = false;
	for (int i = 0; i < PyCommandLine.Len(); i++)
	{
		if(PyCommandLine[i] == ' ')
		{
			PyArgv.Add(FString());
			continue;
		}
		else if(PyCommandLine[i] == '\"' && !escaped)
		{
			i++;
			while(i < PyCommandLine.Len() && !(PyCommandLine[i] == '"'))
			{
				PyArgv[PyArgv.Num() - 1].AppendChar(PyCommandLine[i]);
				i++;
				if (i == PyCommandLine.Len())
				{
					PyArgv[PyArgv.Num() - 1].InsertAt(0, "\"");
				}
			}
		}
		else
		{
			if (PyCommandLine[i] == '\\')
				escaped = true;
			else
				escaped = false;
			PyArgv[PyArgv.Num() - 1].AppendChar(PyCommandLine[i]);
		}
	}
	PyArgv.Insert(Filepath, 0);

#if PY_MAJOR_VERSION >= 3
	wchar_t **argv = (wchar_t **)malloc(PyArgv.Num() * sizeof(void*));
#else
	char **argv = (char **)malloc(PyArgv.Num() * sizeof(void*));
#endif

	for (int i=0; i<PyArgv.Num(); i++)
	{
#if PY_MAJOR_VERSION >= 3
		argv[i] = (wchar_t*)malloc(PyArgv[i].Len()+1);
#if defined(UNREAL_ENGINE_PYTHON_ON_MAC) || defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
		wcsncpy(argv[i], *PyArgv[i].ReplaceEscapedCharWithChar(), PyArgv[i].Len() + 1);
#else
		wcscpy_s(argv[i], PyArgv[i].Len() + 1, *PyArgv[i].ReplaceEscapedCharWithChar());
#endif
#else
		argv[i] = (char*)malloc(PyArgv[i].Len() + 1);
#if defined(UNREAL_ENGINE_PYTHON_ON_MAC) || defined(UNREAL_ENGINE_PYTHON_ON_LINUX)
		strncpy(argv[i], TCHAR_TO_UTF8(*PyArgv[i].ReplaceEscapedCharWithChar()), PyArgv[i].Len()+1);
#else
		strcpy_s(argv[i], PyArgv[i].Len()+1, TCHAR_TO_UTF8(*PyArgv[i].ReplaceEscapedCharWithChar()));
#endif
#endif
	}

	PySys_SetArgv(PyArgv.Num(), argv);

	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.BrutalFinalize = true;
	PythonModule.RunFile(TCHAR_TO_UTF8(*Filepath));
	return 0;
}
