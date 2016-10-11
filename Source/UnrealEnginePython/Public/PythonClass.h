#pragma once


#include "PythonClass.generated.h"

UCLASS()
class UPythonClass : public UClass
{
	GENERATED_BODY()

public:

	void SetPyConstructor(PyObject *callable) {
		py_constructor = callable;
		Py_INCREF(py_constructor);
	}

	void CallPyConstructor(UObject *u_obj) {
		if (!py_constructor)
			return;
		PyObject *self = (PyObject *)ue_get_python_wrapper(u_obj);
		if (!self) {
			unreal_engine_py_log_error();
			return;
		}
		PyObject *ret = PyObject_CallObject(py_constructor, Py_BuildValue("(O)", self));
		if (!ret) {
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

private:
	
	PyObject *py_constructor;
};

