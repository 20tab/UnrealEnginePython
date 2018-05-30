#pragma once

#include "UEPyModule.h"
#include "PythonClass.generated.h"

UCLASS()
class UPythonClass : public UClass
{
	GENERATED_BODY()

public:

	void SetPyConstructor(PyObject *callable)
	{
		py_constructor = callable;
		Py_INCREF(py_constructor);
	}

	void CallPyConstructor(ue_PyUObject *self)
	{
		if (!py_constructor)
			return;
		PyObject *ret = PyObject_CallObject(py_constructor, Py_BuildValue("(O)", self));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	// __dict__ is stored here
	ue_PyUObject *py_uobject;

private:

	PyObject * py_constructor;
};

