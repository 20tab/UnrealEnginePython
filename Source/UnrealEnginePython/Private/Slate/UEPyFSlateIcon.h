#pragma once

#include "UEPyModule.h"

#include "Runtime/SlateCore/Public/Textures/SlateIcon.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FSlateIcon icon;
} ue_PyFSlateIcon;

void ue_python_init_fslate_icon(PyObject *);

ue_PyFSlateIcon *py_ue_new_fslate_icon(const FSlateIcon slate_icon);
ue_PyFSlateIcon *py_ue_is_fslate_icon(PyObject *);
