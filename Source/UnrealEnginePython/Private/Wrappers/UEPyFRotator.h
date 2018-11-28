#pragma once

#include "UnrealEnginePython.h"
#include "UEPyFVector.h"
#include "UEPyFQuat.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FRotator rot;
} ue_PyFRotator;

extern PyTypeObject ue_PyFRotatorType;

PyObject *py_ue_new_frotator(FRotator);
ue_PyFRotator *py_ue_is_frotator(PyObject *);

void ue_python_init_frotator(PyObject *);

bool py_ue_rotator_arg(PyObject *, FRotator &);
