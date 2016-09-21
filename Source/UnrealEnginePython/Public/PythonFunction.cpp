#include "UnrealEnginePythonPrivatePCH.h"
#include "PythonFunction.h"



void UPythonFunction::SetPyCallable(PyObject *callable)
{
	py_callable = callable;
	Py_INCREF(py_callable);
}


void UPythonFunction::CallPythonCallable(FFrame& Stack, RESULT_DECL)
{

	FScopePythonGIL gil;

	UPythonFunction *function = static_cast<UPythonFunction *>(Stack.CurrentNativeFunction);

	if (function->GetSuperFunction()) {
		UE_LOG(LogPython, Warning, TEXT("HAS SUPER FUNCTION"));
	}

	bool on_error = false;

	// count the number of arguments
	Py_ssize_t argn = Stack.Object ? 1 : 0;
	TFieldIterator<UProperty> IArgs(function);
	for (; IArgs && ((IArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++IArgs) {
		argn++;
	}
	UE_LOG(LogPython, Warning, TEXT("Initializing %d parameters"), argn);
	PyObject *py_args = PyTuple_New(argn);

	if (Stack.Object) {
		PyObject *py_obj = (PyObject *)ue_get_python_wrapper(Stack.Object);
		if (!py_obj) {
			unreal_engine_py_log_error();
			on_error = true;
		}
		else {
			PyTuple_SetItem(py_args, 0, py_obj);
		}
	}

	uint8 *frame = (uint8 *)FMemory_Alloca(function->PropertiesSize);
	FMemory::Memzero(frame, function->PropertiesSize);

	argn = Stack.Object ? 1 : 0;
	// is it a bluepritn call ?
	if (*Stack.Code == EX_EndFunctionParms && function->NumParms > 0) {
		for (UProperty *prop = (UProperty *)function->Children; prop; prop = (UProperty *)prop->Next) {
			if (!on_error) {
				PyObject *arg = ue_py_convert_property(prop, (uint8 *)Stack.Locals);
				if (!arg) {
					unreal_engine_py_log_error();
					on_error = true;
				}
				else {
					PyTuple_SetItem(py_args, argn++, arg);
				}
			}
		}
	}
	else {
		for (UProperty *prop = (UProperty *)function->Children; *Stack.Code != EX_EndFunctionParms; prop = (UProperty *)prop->Next) {
			Stack.Step(Stack.Object, prop->ContainerPtrToValuePtr<uint8>(frame));
			if (!on_error) {
				PyObject *arg = ue_py_convert_property(prop, frame);
				if (!arg) {
					unreal_engine_py_log_error();
					on_error = true;
				}
				else {
					PyTuple_SetItem(py_args, argn++, arg);
				}
			}
		}
	}

	Stack.Code++;

	if (on_error || !function->py_callable) {
		Py_DECREF(py_args);
		return;
	}

	PyObject *ret = PyObject_CallObject(function->py_callable, py_args);
	Py_DECREF(py_args);
	if (!ret) {
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);


	return;

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