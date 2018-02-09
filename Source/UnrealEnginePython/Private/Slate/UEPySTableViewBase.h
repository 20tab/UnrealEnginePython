#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Views/STableViewBase.h"

extern PyTypeObject ue_PySTableViewBaseType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySTableViewBase;

void ue_python_init_stable_view_base(PyObject *);

ue_PySTableViewBase *py_ue_is_stable_view_base(PyObject *);