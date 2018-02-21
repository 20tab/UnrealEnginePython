#pragma once

#include "UnrealEnginePython.h"

extern PyTypeObject ue_PySWidgetType;

class UPythonSlateDelegate;
typedef struct ue_PySWidget ue_PySWidget;

struct ue_PySWidget
{
	PyObject_HEAD
	/* Type-specific fields go here. */

	TSharedRef<SWidget> s_widget;
	PyObject *py_dict;
};

void ue_python_init_swidget(PyObject *);

ue_PySWidget *py_ue_is_swidget(PyObject *);

