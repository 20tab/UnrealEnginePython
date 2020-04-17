#pragma once

#include "UEPySCompoundWidget.h"

#include "Runtime/SlateCore/Public/Widgets/SWindow.h"

extern PyTypeObject ue_PySWindowType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySWindow;

void ue_python_init_swindow(PyObject *);

ue_PySWindow *py_ue_is_swindow(PyObject *obj);

ue_PySWindow *py_ue_new_swindow(TSharedRef<SWindow>);