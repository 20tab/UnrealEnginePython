#pragma once

#include "UEPySCompoundWidget.h"

#if WITH_EDITOR

#include "Editor/UnrealEd/Public/SEditorViewport.h"

extern PyTypeObject ue_PySEditorViewportType;

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySEditorViewport;

void ue_python_init_seditor_viewport(PyObject *);
#endif
