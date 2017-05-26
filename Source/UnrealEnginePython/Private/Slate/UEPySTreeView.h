#pragma once

#include "UnrealEnginePython.h"

#include "UEPySListView.h"

#include "Runtime/Slate/Public/Widgets/Views/STreeView.h"

extern PyTypeObject ue_PySTreeViewType;

typedef struct {
	ue_PySListView s_list_view;
	/* Type-specific fields go here. */
} ue_PySTreeView;

void ue_python_init_stree_view(PyObject *);
