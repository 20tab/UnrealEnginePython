#pragma once


#include "UEPySCompoundWidget.h"

#include "Runtime/Slate/Public/Widgets/Input/SNumericEntryBox.h"

extern PyTypeObject ue_PySNumericEntryBoxType;

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySNumericEntryBox;

void ue_python_init_snumeric_entry_box(PyObject *);
