#pragma once

#include "UnrealEnginePython.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyFSlateApplication;

void ue_python_init_fslate_application(PyObject *);
