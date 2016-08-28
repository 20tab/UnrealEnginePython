#pragma once



#include "UnrealEnginePython.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FTimerHandle thandle;
	PyObject *py_callable;
	UWorld *world;
} ue_PyFTimerHandle;

PyObject *py_ue_set_timer(ue_PyUObject *, PyObject *);

void ue_python_init_ftimerhandle(PyObject *);