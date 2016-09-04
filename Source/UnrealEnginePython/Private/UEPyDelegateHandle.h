#pragma once

#include "UnrealEnginePython.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FDelegateHandle dhandle;
	PyObject *py_callable;
} ue_PyFDelegateHandle;
  
void ue_python_init_fdelegatehandle(PyObject *);