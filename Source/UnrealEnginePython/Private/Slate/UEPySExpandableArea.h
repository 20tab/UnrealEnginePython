#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySExpandableArea;

void ue_python_init_sexpandable_area(PyObject *);
