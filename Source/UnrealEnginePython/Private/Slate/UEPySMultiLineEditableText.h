#pragma once

#include "UEPySWidget.h"

#include "Runtime/Slate/Public/Widgets/Text/SMultiLineEditableText.h"

extern PyTypeObject ue_PySMultiLineEditableTextType;

typedef struct
{
	ue_PySWidget s_widget;
	/* Type-specific fields go here. */
} ue_PySMultiLineEditableText;

void ue_python_init_smulti_line_editable_text(PyObject *);
