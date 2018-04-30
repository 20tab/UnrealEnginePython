#pragma once

#include "UnrealEnginePython.h"

#include "Tests/AutomationEditorCommon.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyFAutomationEditorCommonUtils;

void ue_python_init_fautomation_editor_common_utils(PyObject *);
