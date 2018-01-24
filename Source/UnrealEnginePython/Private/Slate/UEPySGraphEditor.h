#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
#include "UEPySCompoundWidget.h"

#include "Editor/UnrealEd/Public/GraphEditor.h"

extern PyTypeObject ue_PySGraphEditorType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySGraphEditor;

void ue_python_init_sgraph_editor(PyObject *);
#endif