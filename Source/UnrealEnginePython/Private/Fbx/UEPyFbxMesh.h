#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12

#include <fbxsdk.h>

struct ue_PyFbxMesh
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FbxMesh *fbx_mesh;
};


void ue_python_init_fbx_mesh(PyObject *);

PyObject *py_ue_new_fbx_mesh(FbxMesh *);

#endif
#endif
