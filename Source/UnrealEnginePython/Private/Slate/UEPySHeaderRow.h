#pragma once

#include "UnrealEnginePython.h"

#include "UEPySBorder.h"

#include "Runtime/Slate/Public/Widgets/Views/SHeaderRow.h"

extern PyTypeObject ue_PySHeaderRowType;

typedef struct {
	ue_PySBorder s_border;
	/* Type-specific fields go here. */
} ue_PySHeaderRow;

void ue_python_init_sheader_row(PyObject *);
