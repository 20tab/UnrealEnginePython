#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Input/SCheckBox.h"

extern PyTypeObject ue_PySCheckBoxType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySCheckBox;

void ue_python_init_scheck_box(PyObject *);
