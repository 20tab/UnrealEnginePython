#pragma once

#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"

extern PyTypeObject ue_PySTextBlockType;

typedef struct
{
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
} ue_PySTextBlock;

void ue_python_init_stext_block(PyObject *);
