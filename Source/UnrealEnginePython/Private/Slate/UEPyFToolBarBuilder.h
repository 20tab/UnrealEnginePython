#pragma once

#include "UEPySlate.h"

#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FToolBarBuilder tool_bar_builder;
} ue_PyFToolBarBuilder;

void ue_python_init_ftool_bar_builder(PyObject *);

PyObject *py_ue_new_ftool_bar_builder(FToolBarBuilder);
