// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "UnrealEnginePythonPrivatePCH.h"

void unreal_engine_init_py_module();

#if UNREAL_ENGINE_PYTHON_ON_LINUX
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
#if UEPY_THREADING
	if (PyGILState_Check() == 1) {
		ue_python_gil = PyEval_SaveThread();
	}
#endif
}

bool FUnrealEnginePythonModule::PythonGILAcquire() {
#if UEPY_THREADING
	if (PyGILState_Check() == 0) {
		PyEval_RestoreThread((PyThreadState *)ue_python_gil);
		return true;
	}
	return false;
#endif
	return true;
}

void FUnrealEnginePythonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	Py_Initialize();
#if PY_MAJOR_VERSION >= 3
	wchar_t *argv[] = { UTF8_TO_TCHAR("UnrealEngine"), NULL };
#else
	char *argv[] = { (char *)"UnrealEngine", NULL };
#endif
	PySys_SetArgv(1, argv);

	PyEval_InitThreads();

	unreal_engine_init_py_module();

	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_path = PyDict_GetItemString(py_sys_dict, "path");

	char *zip_path = TCHAR_TO_UTF8(*FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("ue_python.zip")));
	PyObject *py_zip_path = PyUnicode_FromString(zip_path);
	PyList_Insert(py_path, 0, py_zip_path);

	char *scripts_path = TCHAR_TO_UTF8(*FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("Scripts")));
	PyObject *py_scripts_path = PyUnicode_FromString(scripts_path);
	PyList_Insert(py_path, 0, py_scripts_path);


	UE_LOG(LogPython, Log, TEXT("Python VM initialized: %s"), UTF8_TO_TCHAR(Py_GetVersion()));
	UE_LOG(LogPython, Log, TEXT("Python Scripts search path: %s"), UTF8_TO_TCHAR(scripts_path));

	PyObject *main_module = PyImport_AddModule("__main__");
	main_dict = PyModule_GetDict(main_module);
	local_dict = main_dict;// PyDict_New();

	if (PyImport_ImportModule("ue_site")) {
		UE_LOG(LogPython, Log, TEXT("ue_site Python module successfully imported"));
	}
	else {
		// TODO gracefully manage the error
		unreal_engine_py_log_error();
	}

#if WITH_EDITOR
	// register commands (after importing ue_site)
	FPythonSlateCommands::Register();
	// apply extenders
	FPythonSlateCommands::ApplyPythonExtenders();
#endif

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

void FUnrealEnginePythonModule::RunFile(char *filename) {
	FScopePythonGIL gil;
	char *full_path = filename;
	if (!FPaths::FileExists(filename))
	{
		full_path = TCHAR_TO_UTF8(*FPaths::Combine(*FPaths::GameContentDir(), UTF8_TO_TCHAR("Scripts"), *FString("/"), UTF8_TO_TCHAR(filename)));
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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealEnginePythonModule, UnrealEnginePython)

