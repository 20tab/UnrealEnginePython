// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealEnginePythonPrivatePCH.h"

void unreal_engine_init_py_module();

#define LOCTEXT_NAMESPACE "FUnrealEnginePythonModule"


void FUnrealEnginePythonModule::PythonGILRelease() {
#if UEPY_THREADING
	ue_python_gil = PyEval_SaveThread();
#endif
}

void FUnrealEnginePythonModule::PythonGILAcquire() {
#if UEPY_THREADING
	PyEval_RestoreThread((PyThreadState *)ue_python_gil);
#endif
}

void FUnrealEnginePythonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	//First we need to initialize python and manually setup our environment.
	//We are manually setting up the environment due to no easy way to redirect python to a zip install.
	FString scripts_path = FPaths::Combine(*FPaths::ConvertRelativePathToFull(FPaths::GameContentDir()), UTF8_TO_TCHAR("Scripts"));
	FString home_path = FString::Printf(TEXT("%s/python%d%d"), *scripts_path, PY_MAJOR_VERSION, PY_MINOR_VERSION);
	
	Py_NoSiteFlag = 1;
	Py_SetProgramName("UnrealEngine");
	Py_SetPythonHome(TCHAR_TO_UTF8(*home_path));
	Py_InitializeEx(0);

	PyObject *main_module = PyImport_AddModule("__main__");
	FString startup_cmd = FString::Printf(TEXT("import sys;sys.path.append('%s');"), *scripts_path);
	PyRun_SimpleString(TCHAR_TO_UTF8(*startup_cmd));

	PyEval_InitThreads();

	unreal_engine_init_py_module();

	// Python implemented startup script.
	PyObject *ue_module = PyImport_ImportModule("unreal_engine");

	if (!PyErr_Occurred()) {
		PyObject_SetAttrString(main_module, "unreal_engine", ue_module);
		Py_XDECREF(ue_module);
	}
	else {
		// TODO gracefully manage the error
		unreal_engine_py_log_error();
	}

	PyRun_SimpleString("import site");
	// release the GIL
	PythonGILRelease();
	UE_LOG(LogPython, Log, TEXT("Python VM initialized: %s"), UTF8_TO_TCHAR(Py_GetVersion()));
	UE_LOG(LogPython, Log, TEXT("Python Scripts search path: %s"), *home_path);
}

void FUnrealEnginePythonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogPython, Log, TEXT("Goodbye Python"));
	PythonGILAcquire();
	Py_Finalize();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePythonModule, UnrealEnginePython)

