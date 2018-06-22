#pragma once

#include "UnrealEnginePython.h"


#include "UEPySPythonMultiColumnTableRow.h"

#include "Runtime/Slate/Public/Widgets/Views/SExpanderArrow.h"

extern PyTypeObject ue_PySExpanderArrowType;

typedef struct {
    ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
    ue_PySPythonMultiColumnTableRow* owner_row_py;
} ue_PySExpanderArrow;

void ue_python_init_sexpander_arrow(PyObject *);
