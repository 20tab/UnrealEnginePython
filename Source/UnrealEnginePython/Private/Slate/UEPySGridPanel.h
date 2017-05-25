#pragma once

#include "UnrealEnginePython.h"


#include "UEPySPanel.h"

#include "Runtime/Slate/Public/Widgets/Layout/SGridPanel.h"

extern PyTypeObject ue_PySGridPanelType;

typedef struct {
	ue_PySPanel s_panel;
	/* Type-specific fields go here. */
} ue_PySGridPanel;

void ue_python_init_sgrid_panel(PyObject *);

