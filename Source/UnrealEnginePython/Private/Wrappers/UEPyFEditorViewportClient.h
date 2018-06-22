#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include "Wrappers/UEPyFViewportClient.h"


#include "Editor/UnrealEd/Public/EditorViewportClient.h"

struct ue_PyFEditorViewportClient
{
	ue_PyFViewportClient viewport_client;
	/* Type-specific fields go here. */
	TSharedRef<FEditorViewportClient> editor_viewport_client;;
};

void ue_python_init_feditor_viewport_client(PyObject *);

PyObject *py_ue_new_feditor_viewport_client(TSharedRef<FEditorViewportClient>);

ue_PyFEditorViewportClient *py_ue_is_feditor_viewport_client(PyObject *);

#endif