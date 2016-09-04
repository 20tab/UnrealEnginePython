#pragma once



#include "UnrealEnginePython.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FHitResult hit;
} ue_PyFHitResult;

PyObject *py_ue_new_fhitresult(FHitResult);
ue_PyFHitResult *py_ue_is_fhitresult(PyObject *);

void ue_python_init_fhitresult(PyObject *);