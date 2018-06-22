#pragma once

#include "UEPySCompoundWidget.h"

#if WITH_EDITOR

#include "Editor/PropertyEditor/Public/PropertyCustomizationHelpers.h"

extern PyTypeObject ue_PySObjectPropertyEntryBoxType;

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySObjectPropertyEntryBox;

void ue_python_init_sobject_property_entry_box(PyObject *);
#endif
