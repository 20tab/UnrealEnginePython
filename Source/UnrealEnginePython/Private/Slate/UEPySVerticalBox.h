#pragma once

#include "UEPySBoxPanel.h"

#include "Runtime/SlateCore/Public/Widgets/SBoxPanel.h"

extern PyTypeObject ue_PySVerticalBoxType;

typedef struct
{
	ue_PySBoxPanel s_box_panel;
	/* Type-specific fields go here. */
} ue_PySVerticalBox;

void ue_python_init_svertical_box(PyObject *);
