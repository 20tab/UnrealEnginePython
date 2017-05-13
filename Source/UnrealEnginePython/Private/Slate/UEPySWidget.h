#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

extern PyTypeObject ue_PySWidgetType;

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	TSharedRef<SWidget> s_widget;
	PyObject *py_dict;
} ue_PySWidget;

void ue_python_init_swidget(PyObject *);

ue_PySWidget *py_ue_is_swidget(PyObject *);
#endif
