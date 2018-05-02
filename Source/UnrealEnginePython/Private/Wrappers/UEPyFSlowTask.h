#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

struct ue_PyFSlowTask {
	PyObject_HEAD
	/* Type-specific fields go here. */

	FSlowTask slowtask;
};

void ue_python_init_fslowtask(PyObject *);

ue_PyFSlowTask *py_ue_is_fslowtask(PyObject *);

#endif
