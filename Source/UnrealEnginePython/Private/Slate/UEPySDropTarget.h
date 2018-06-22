#pragma once

#include "UEPySCompoundWidget.h"

#if WITH_EDITOR

#include "Editor/EditorWidgets/Public/SDropTarget.h"

extern PyTypeObject ue_PySDropTargetType;

typedef struct {
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySDropTarget;

void ue_python_init_sdrop_target(PyObject *);
#endif
