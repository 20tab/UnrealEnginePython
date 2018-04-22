#pragma once

#include "UEPySDropTarget.h"

#if WITH_EDITOR

#include "Editor/EditorWidgets/Public/SAssetDropTarget.h"

extern PyTypeObject ue_PySAssetDropTargetType;

typedef struct {
	ue_PySDropTarget s_drop_target;
	/* Type-specific fields go here. */
} ue_PySAssetDropTarget;

void ue_python_init_sasset_drop_target(PyObject *);
#endif
