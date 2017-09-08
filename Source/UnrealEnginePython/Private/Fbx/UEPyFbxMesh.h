#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

#include <fbxsdk.h>

struct ue_PyFbxMesh
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxMesh *fbx_mesh;
};


void ue_python_init_fbx_mesh(PyObject *);

PyObject *py_ue_new_fbx_mesh(FbxNode *);

#endif
