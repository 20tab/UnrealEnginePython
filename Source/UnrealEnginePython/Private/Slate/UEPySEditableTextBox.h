#pragma once

#include "UEPySBorder.h"

#include "Runtime/Slate/Public/Widgets/Input/SEditableTextBox.h"

extern PyTypeObject ue_PySEditableTextBoxType;

typedef struct {
	ue_PySBorder s_border;
	/* Type-specific fields go here. */
} ue_PySEditableTextBox;

void ue_python_init_seditable_text_box(PyObject *);

