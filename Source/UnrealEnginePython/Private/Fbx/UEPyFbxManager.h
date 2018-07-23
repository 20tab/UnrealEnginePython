#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if PLATFORM_LINUX
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
#endif


#include <fbxsdk.h>

struct ue_PyFbxManager
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FbxManager *fbx_manager;
};


void ue_python_init_fbx_manager(PyObject *);

ue_PyFbxManager *py_ue_is_fbx_manager(PyObject *);

#endif
