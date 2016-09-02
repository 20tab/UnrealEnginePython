// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealEnginePythonPrivatePCH.h"

void unreal_engine_init_py_module();


#define LOCTEXT_NAMESPACE "FUnrealEnginePythonModule"


void FUnrealEnginePythonModule::PythonGILRelease() {
	ue_python_gil = PyEval_SaveThread();
}

void FUnrealEnginePythonModule::PythonGILAcquire() {
	PyEval_RestoreThread((PyThreadState *)ue_python_gil);
}

void FUnrealEnginePythonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	Py_Initialize();
	wchar_t *argv[] = { UTF8_TO_TCHAR("UnrealEngine"), NULL };
	PySys_SetArgv(1, argv);

	PyEval_InitThreads();

	unreal_engine_init_py_module();

	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_path = PyDict_GetItemString(py_sys_dict, "path");

	char *zip_path = TCHAR_TO_UTF8(*FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("python35.zip")));
	PyObject *py_zip_path = PyUnicode_FromString(zip_path);
	PyList_Insert(py_path, 0, py_zip_path);

	char *scripts_path = TCHAR_TO_UTF8(*FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("Scripts")));
	PyObject *py_scripts_path = PyUnicode_FromString(scripts_path);
	PyList_Insert(py_path, 0, py_scripts_path);


	UE_LOG(LogPython, Log, TEXT("Python VM initialized: %s"), UTF8_TO_TCHAR(Py_GetVersion()));
	UE_LOG(LogPython, Log, TEXT("Python Scripts search path: %s"), UTF8_TO_TCHAR(scripts_path));

	if (PyImport_ImportModule("ue_site")) {
		UE_LOG(LogPython, Log, TEXT("ue_site Python module successfully imported"));
	}
	else {
		unreal_engine_py_log_error();
	}

	// TODO investigate python threads support
	//PythonGILRelease();
}

void FUnrealEnginePythonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UE_LOG(LogPython, Log, TEXT("Goodbye Python"));
	Py_Finalize();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePythonModule, UnrealEnginePython)

