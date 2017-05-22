#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "UEPySTableViewBase.h"

#include "Runtime/Slate/Public/Widgets/Views/SListView.h"

extern PyTypeObject ue_PySListViewType;

typedef struct {
	ue_PySTableViewBase s_table_view_base;
	/* Type-specific fields go here. */
} ue_PySListView;

void ue_python_init_slist_view(PyObject *);

#endif