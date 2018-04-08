#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		UFunction *u_function;
	UObject *u_target;
} ue_PyCallable;

PyObject *py_ue_new_callable(UFunction *, UObject *);
ue_PyCallable *py_ue_is_callable(PyObject *);

void ue_python_init_callable(PyObject *);