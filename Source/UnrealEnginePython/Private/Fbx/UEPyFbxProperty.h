#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if PLATFORM_LINUX
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
#endif

#include <fbxsdk.h>

struct ue_PyFbxProperty {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FbxProperty fbx_property;
};


void ue_python_init_fbx_property(PyObject *);

PyObject *py_ue_new_fbx_property(FbxProperty);

ue_PyFbxProperty *py_ue_is_fbx_property(PyObject *);
#endif
