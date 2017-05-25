#pragma once

#include "UnrealEnginePython.h"


#include "UEPySBoxPanel.h"

#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"

extern PyTypeObject ue_PySHorizontalBoxType;

typedef struct {
	ue_PySBoxPanel s_box_panel;
	/* Type-specific fields go here. */
} ue_PySHorizontalBox;

void ue_python_init_shorizontal_box(PyObject *);
