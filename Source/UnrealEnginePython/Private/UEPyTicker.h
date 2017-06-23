#pragma once

#include "UnrealEnginePython.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FDelegateHandle dhandle;
	UPythonDelegate *py_delegate;
	bool garbaged;
} ue_PyFDelegateHandle;

PyObject *py_unreal_engine_add_ticker(PyObject *, PyObject *);
PyObject *py_unreal_engine_remove_ticker(PyObject *, PyObject *);

void ue_python_init_fdelegatehandle(PyObject *);