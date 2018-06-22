#pragma once
#include "UnrealEnginePython.h"
#include "Misc/SlowTask.h"

#if WITH_EDITOR

typedef struct  {
	PyObject_HEAD
	/* Type-specific fields go here. */

	FSlowTask slowtask;
} ue_PyFSlowTask;

void ue_python_init_fslowtask(PyObject *);

ue_PyFSlowTask *py_ue_is_fslowtask(PyObject *);

#endif
