#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FMenuBuilder *menu_builder;
} ue_PyFMenuBuilder;

void ue_python_init_fmenu_builder(PyObject *);

#endif