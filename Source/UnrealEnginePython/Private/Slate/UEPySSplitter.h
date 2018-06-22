#pragma once

#include "UEPySPanel.h"

#include "Runtime/Slate/Public/Widgets/Layout/SSplitter.h"

extern PyTypeObject ue_PySSplitterType;

typedef struct
{
	ue_PySPanel s_panel;
	/* Type-specific fields go here. */
} ue_PySSplitter;

void ue_python_init_ssplitter(PyObject *);
