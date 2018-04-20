#pragma once

#include "UEPyModule.h"

struct FPythonItem
{
	PyObject *py_object = nullptr;

	FPythonItem(PyObject *item)
	{
		py_object = item;
	}
};