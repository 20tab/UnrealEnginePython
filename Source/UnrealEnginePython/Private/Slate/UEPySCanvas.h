#pragma once

#include "UnrealEnginePython.h"


#include "UEPySPanel.h"

#include "Runtime/Slate/Public/Widgets/SCanvas.h"

extern PyTypeObject ue_PySCanvasType;

typedef struct {
	ue_PySPanel s_panel;
	/* Type-specific fields go here. */
} ue_PySCanvas;

void ue_python_init_scanvas(PyObject *);
