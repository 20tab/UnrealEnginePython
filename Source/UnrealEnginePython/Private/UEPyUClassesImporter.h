#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyUClassesImporter;

PyObject *py_ue_new_uclassesimporter();

void ue_python_init_uclassesimporter(PyObject *);