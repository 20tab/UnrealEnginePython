#pragma once

#include "UEPySBorder.h"

#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"

extern PyTypeObject ue_PySDockTabType;

typedef struct
{
	ue_PySBorder s_border;
	/* Type-specific fields go here. */
} ue_PySDockTab;

void ue_python_init_sdock_tab(PyObject *);
