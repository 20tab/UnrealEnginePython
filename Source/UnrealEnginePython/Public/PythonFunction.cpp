#include "UnrealEnginePythonPrivatePCH.h"
#include "PythonFunction.h"



void UPythonFunction::SetPyCallable(PyObject *callable)
{
	py_callable = callable;
	Py_INCREF(py_callable);
}


void UPythonFunction::CallPythonCallable(FFrame& Stack, RESULT_DECL)
{
	UPythonFunction *function = static_cast<UPythonFunction *>(Stack.CurrentNativeFunction);

	P_FINISH;

	if (!function->py_callable)
		return;

	PyObject *ret = PyObject_CallObject(function->py_callable, NULL);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

UPythonFunction::~UPythonFunction()
{
	Py_XDECREF(py_callable);
#if UEPY_MEMORY_DEBUG
	UE_LOG(LogPython, Warning, TEXT("PythonFunction callable XDECREF'ed"));
#endif
}