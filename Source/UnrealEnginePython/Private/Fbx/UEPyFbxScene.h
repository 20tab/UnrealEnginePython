#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if PLATFORM_LINUX
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
#endif

#include <fbxsdk.h>

struct ue_PyFbxScene {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxScene *fbx_scene;
};


void ue_python_init_fbx_scene(PyObject *);

ue_PyFbxScene *py_ue_is_fbx_scene(PyObject *);

#endif
