#pragma once

#include "UEPySBorder.h"

#include "Runtime/Slate/Public/Widgets/Input/SButton.h"

extern PyTypeObject ue_PySButtonType;

typedef struct {
	ue_PySBorder s_border;
	/* Type-specific fields go here. */
} ue_PySButton;

void ue_python_init_sbutton(PyObject *);
