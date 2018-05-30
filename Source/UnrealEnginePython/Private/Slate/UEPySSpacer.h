#pragma once

#include "UEPyModule.h"


#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Layout/SSpacer.h"

extern PyTypeObject ue_PySSpacerType;

typedef struct {
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
} ue_PySSpacer;

void ue_python_init_sspacer(PyObject *);
