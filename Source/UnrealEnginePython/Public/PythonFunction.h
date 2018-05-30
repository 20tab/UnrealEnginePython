#pragma once

#include "UnrealEnginePython.h"
#include "PythonFunction.generated.h"

UCLASS()
class UPythonFunction : public UFunction
{
	GENERATED_BODY()

public:
	~UPythonFunction();
	void SetPyCallable(PyObject *callable);

	DECLARE_FUNCTION(CallPythonCallable);

	PyObject *py_callable;
};

