#pragma once

#include "UEPyModule.h"

#if WITH_EDITOR

#include "Editor/MaterialEditor/Public/MaterialEditorUtilities.h"
#include "Editor/MaterialEditor/Public/MaterialEditorActions.h"
#if ENGINE_MAJOR_VERSION == 4
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"
#endif
#include "Editor/MaterialEditor/Public/IMaterialEditor.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
} ue_PyFMaterialEditorUtilities;

void ue_python_init_fmaterial_editor_utilities(PyObject *);
#endif
