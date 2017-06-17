#pragma once

#include "UnrealEnginePython.h"


#include "UEPySPanel.h"

#include "Runtime/Slate/Public/Widgets/Layout/SBox.h"

extern PyTypeObject ue_PySBoxType;

typedef struct {
	ue_PySPanel s_panel;
	/* Type-specific fields go here. */
} ue_PySBox;

void ue_python_init_sbox(PyObject *);
