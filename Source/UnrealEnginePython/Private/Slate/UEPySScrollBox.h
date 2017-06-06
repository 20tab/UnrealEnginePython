#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Layout/SScrollBox.h"

extern PyTypeObject ue_PySScrollBoxType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySScrollBox;

void ue_python_init_sscroll_box(PyObject *);
