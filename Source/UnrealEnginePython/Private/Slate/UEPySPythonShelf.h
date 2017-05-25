#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

extern PyTypeObject ue_PySPythonShelfType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySPythonShelf;

void ue_python_init_spython_shelf(PyObject *);
