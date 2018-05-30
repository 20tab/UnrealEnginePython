#pragma once

#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Input/SSlider.h"

extern PyTypeObject ue_PySSliderType;

typedef struct
{
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
} ue_PySSlider;

void ue_python_init_sslider(PyObject *);
