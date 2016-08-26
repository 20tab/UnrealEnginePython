#include "UnrealEnginePythonPrivatePCH.h"
#include "PythonDelegate.h"



void UPythonDelegate::SetPyCallable(PyObject *callable)
{
	py_callable = callable;
	Py_INCREF(py_callable);
}

void UPythonDelegate::SetSignature(UFunction *original_signature)
{
	signature = original_signature;
}

void UPythonDelegate::ProcessEvent(UFunction *function, void *Parms)
{

	if (!py_callable)
		return;

	PyObject *py_args = PyTuple_New(signature->NumParms);
	Py_ssize_t argn = 0;

	TFieldIterator<UProperty> PArgs(signature);
	for (; PArgs && argn < signature->NumParms && ((PArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++PArgs) {
		UProperty *prop = *PArgs;
		PyObject *arg = ue_py_convert_property(prop, (uint8 *)Parms);
		if (!arg) {
			unreal_engine_py_log_error();
			Py_DECREF(py_args);
			return;
		}
		PyTuple_SetItem(py_args, argn, arg);
		argn++;
	}

	PyObject *ret = PyObject_CallObject(py_callable, py_args);
	Py_DECREF(py_args);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void UPythonDelegate::PyFakeCallable()
{
}

void UPythonDelegate::PyInputHandler()
{
	PyObject *ret = PyObject_CallObject(py_callable, NULL);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

UPythonDelegate::~UPythonDelegate()
{
	Py_XDECREF(py_callable);
#if UEPY_MEMORY_DEBUG
	UE_LOG(LogPython, Warning, TEXT("PythonDelegate callable XDECREF'ed"));
#endif
}