#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FMenuBuilder *menu_builder;
} ue_PyFMenuBuilder;

void ue_python_init_fmenu_builder(PyObject *);

PyObject *py_ue_new_fmenu_builder(FMenuBuilder *);
