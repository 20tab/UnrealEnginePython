#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Editor/UnrealEd/Public/Toolkits/AssetEditorManager.h"

struct ue_PyIAssetEditorInstance
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	IAssetEditorInstance *editor_instance;
};

void ue_python_init_iasset_editor_instance(PyObject *);

PyObject *py_ue_new_iasset_editor_instance(IAssetEditorInstance *);

ue_PyIAssetEditorInstance *py_ue_is_iasset_editor_instance(PyObject *);

#endif