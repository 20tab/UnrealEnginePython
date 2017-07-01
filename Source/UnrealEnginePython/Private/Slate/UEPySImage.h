#pragma once

#include "UnrealEnginePython.h"


#include "UEPySLeafWidget.h"

#include "Runtime/Slate/Public/Widgets/Images/SImage.h"
#include "Runtime/SlateCore/Public/Styling/SlateBrush.h"

extern PyTypeObject ue_PySImageType;

typedef struct {
	ue_PySLeafWidget s_leaf_widget;
	/* Type-specific fields go here. */
	FSlateBrush brush;
} ue_PySImage;

void ue_python_init_simage(PyObject *);
