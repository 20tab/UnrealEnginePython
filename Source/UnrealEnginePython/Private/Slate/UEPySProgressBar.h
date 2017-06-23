#pragma once

#include "UnrealEnginePython.h"


#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Notifications/SProgressBar.h"

extern PyTypeObject ue_PySProgressBarType;

typedef struct {
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
} ue_PySProgressBar;

void ue_python_init_sprogress_bar(PyObject *);
