#pragma once

#include "UEPySEditorViewport.h"

#if WITH_EDITOR

#include "Editor/LevelEditor/Public/SLevelViewport.h"
#include "Editor/LevelEditor/Private/SLevelEditor.h"
#include "Editor/UnrealEd/Public/EditorViewportClient.h"
#include "Runtime/Engine/Public/PreviewScene.h"

extern PyTypeObject ue_PySLevelViewportType;

typedef struct
{
	ue_PySEditorViewport s_editor_viewport;
	/* Type-specific fields go here. */
} ue_PySLevelViewport;

void ue_python_init_slevel_viewport(PyObject *);
#endif