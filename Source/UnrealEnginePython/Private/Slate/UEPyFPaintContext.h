#pragma once

#include "UEPyModule.h"

#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FPaintContext paint_context;
} ue_PyFPaintContext;

void ue_python_init_fpaint_context(PyObject *);

PyObject *py_ue_new_fpaint_context(FPaintContext);
