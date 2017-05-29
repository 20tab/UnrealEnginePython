#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/SlateCore/Public/Styling/SlateStyle.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FSlateStyleSet *style_set;
} ue_PyFSlateStyleSet;

void ue_python_init_fslate_style_set(PyObject *);
