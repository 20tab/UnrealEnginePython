#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Layout/SBorder.h"

extern PyTypeObject ue_PySBorderType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySBorder;

void ue_python_init_sborder(PyObject *);
