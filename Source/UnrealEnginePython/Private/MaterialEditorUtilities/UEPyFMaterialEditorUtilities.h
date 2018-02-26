#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Editor/MaterialEditor/Public/MaterialEditorUtilities.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyFMaterialEditorUtilities;

void ue_python_init_fmaterial_editor_utilities(PyObject *);
#endif
