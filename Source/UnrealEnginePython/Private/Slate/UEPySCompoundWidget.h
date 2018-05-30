#pragma once

#include "UEPySWidget.h"

extern PyTypeObject ue_PySCompoundWidgetType;

typedef struct
{
	ue_PySWidget s_widget;
	/* Type-specific fields go here. */
} ue_PySCompoundWidget;

void ue_python_init_scompound_widget(PyObject *);

