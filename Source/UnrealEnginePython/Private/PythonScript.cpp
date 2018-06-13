
#include "PythonScript.h"

#include "UEPyModule.h"


static void callback(void *arg)
{
	UPythonScript *self = (UPythonScript *)arg;
	self->CallSpecificFunctionWithArgs();
}

void UPythonScript::Run()
{
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(TCHAR_TO_UTF8(*ScriptPath));
	if (!FunctionToCall.IsEmpty())
	{
		CallSpecificFunctionWithArgs();
	}
}

void UPythonScript::CallSpecificFunctionWithArgs()
{
	PyObject *function_to_call = PyDict_GetItemString(PyEval_GetGlobals(), TCHAR_TO_UTF8(*FunctionToCall));
	if (!function_to_call)
	{
		UE_LOG(LogPython, Error, TEXT("unable to find function %s"), *FunctionToCall);
		return;
	}

	int n = FunctionArgs.Num();
	PyObject *args = nullptr;

	if (n > 0)
		args = PyTuple_New(n);

	for (int i = 0; i < n; i++)
	{
		PyTuple_SetItem(args, i, PyUnicode_FromString(TCHAR_TO_UTF8(*FunctionArgs[i])));
	}

	PyObject *ret = PyObject_Call(function_to_call, args, nullptr);
	Py_DECREF(args);
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	else
	{
		Py_DECREF(ret);
	}
}