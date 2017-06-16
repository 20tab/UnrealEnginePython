#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include <fbxsdk.h>

struct ue_PyFbxScene {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxScene *fbx_scene;
};


void ue_python_init_fbx_scene(PyObject *);

ue_PyFbxScene *py_ue_is_fbx_scene(PyObject *);

#endif
