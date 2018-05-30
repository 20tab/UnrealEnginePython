#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyUStructsImporter;

PyObject *py_ue_new_ustructsimporter();

void ue_python_init_ustructsimporter(PyObject *);