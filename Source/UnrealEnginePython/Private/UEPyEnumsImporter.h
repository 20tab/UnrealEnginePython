#pragma once



#include "UnrealEnginePython.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
} ue_PyEnumsImporter;

PyObject *py_ue_new_enumsimporter();

void ue_python_init_enumsimporter(PyObject *);