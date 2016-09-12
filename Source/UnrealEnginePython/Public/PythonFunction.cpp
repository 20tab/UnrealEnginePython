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

	UE_LOG(LogPython, Warning, TEXT("CALLING PYTHON FUNCTION"));


	
	if (!Stack.Object) {
		UE_LOG(LogPython, Warning, TEXT("STACK OBJECT NOT DEFINED !!!"));
	}
	else {
		UE_LOG(LogPython, Warning, TEXT("READY TO RUN CALLABLE FOR %s"), *Stack.Object->GetName());
	}

	P_FINISH;

	if (!function->py_callable)
		return;

	FScopePythonGIL gil;

	if (function->GetSuperFunction()) {
		UE_LOG(LogPython, Warning, TEXT("HAS SUPER FUNCTION"));
	}

	// count the number of arguments
	Py_ssize_t argn = 0;
	TFieldIterator<UProperty> IArgs(function);
	for (; IArgs && ((IArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++IArgs) {
		argn++;
	}
	UE_LOG(LogPython, Warning, TEXT("Initializing %d parameters"), argn);
	PyObject *py_args = PyTuple_New(argn);// function->NumParms);

	argn = 0;

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

	PyObject *ret = PyObject_CallObject(function->py_callable, py_args);
	Py_DECREF(py_args);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}

	// get return value (if required)
	UProperty *return_property = ((UFunction *)Stack.Node)->GetReturnProperty();
	if (return_property && function->ReturnValueOffset != MAX_uint16) {
		UE_LOG(LogPython, Warning, TEXT("FOUND RETURN VALUE"));
		ue_py_convert_pyobject(ret, return_property, (uint8 *)Stack.Locals);
		// copy value to stack result value
		FMemory::Memcpy(RESULT_PARAM, Stack.Locals + function->ReturnValueOffset, return_property->ArrayDim * return_property->ElementSize);
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