#pragma once

#include "UEPySWidget.h"

extern PyTypeObject ue_PySPanelType;

typedef struct {
	ue_PySWidget s_widget;
	/* Type-specific fields go here. */
} ue_PySPanel;

void ue_python_init_spanel(PyObject *);

