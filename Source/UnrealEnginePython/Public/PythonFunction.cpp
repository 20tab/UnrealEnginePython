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

	PyObject *py_args = PyTuple_New(function->NumParms);
	Py_ssize_t argn = 0;

	UProperty *return_property = nullptr;

	TFieldIterator<UProperty> PArgs(function);
	for (; PArgs && argn < function->NumParms && ((PArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++PArgs) {
		UProperty *prop = *PArgs;
		PyObject *arg = ue_py_convert_property(prop, (uint8 *)Stack.Locals);
		if (!arg) {
			unreal_engine_py_log_error();
			Py_DECREF(py_args);
			return;
		}
		PyTuple_SetItem(py_args, argn, arg);
		argn++;
	}

	// get return value (if required)
	TFieldIterator<UProperty> RArgs(function);
	for (; RArgs; ++RArgs) {
		UProperty *prop = *RArgs;
		if (prop->GetPropertyFlags() & CPF_ReturnParm) {
			return_property = prop;
			break;
		}
	}

	PyObject *ret = PyObject_CallObject(function->py_callable, py_args);
	Py_DECREF(py_args);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}

	if (return_property) {
		ue_py_convert_pyobject(ret, return_property, (uint8 *)Stack.Locals);
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