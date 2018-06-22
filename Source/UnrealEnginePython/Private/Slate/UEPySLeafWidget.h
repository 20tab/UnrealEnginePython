#pragma once

#include "UEPySWidget.h"

#include "Runtime/SlateCore/Public/Widgets/SLeafWidget.h"

extern PyTypeObject ue_PySLeafWidgetType;

typedef struct {
	ue_PySWidget s_widget;
	/* Type-specific fields go here. */
} ue_PySLeafWidget;

void ue_python_init_sleaf_widget(PyObject *);
