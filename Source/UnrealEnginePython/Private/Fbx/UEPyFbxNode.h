#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12

#if PLATFORM_LINUX
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
#endif

#include <fbxsdk.h>

struct ue_PyFbxNode
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FbxNode *fbx_node;
};


void ue_python_init_fbx_node(PyObject *);

PyObject *py_ue_new_fbx_node(FbxNode *);

ue_PyFbxNode *py_ue_is_fbx_node(PyObject *);

#endif
#endif
