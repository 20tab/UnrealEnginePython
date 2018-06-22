#pragma once

#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Colors/SColorBlock.h"

extern PyTypeObject ue_PySColorBlockType;

typedef struct {
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
} ue_PySColorBlock;

void ue_python_init_scolor_block(PyObject *);
