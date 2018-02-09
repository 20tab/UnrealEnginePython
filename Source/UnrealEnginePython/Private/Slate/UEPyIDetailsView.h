#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
#include "UEPySCompoundWidget.h"

extern PyTypeObject ue_PySCompoundWidgetType;

typedef struct {
    ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PyIDetailsView;

void ue_python_init_idetails_view(PyObject *);

ue_PyIDetailsView * py_ue_is_idetails_view(PyObject *obj);

#endif