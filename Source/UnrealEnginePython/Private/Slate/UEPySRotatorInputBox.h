#pragma once

#include "UnrealEnginePython.h"


#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Input/SRotatorInputBox.h"

extern PyTypeObject ue_PySRotatorInputBoxType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySRotatorInputBox;

void ue_python_init_srotator_input_box(PyObject *);
