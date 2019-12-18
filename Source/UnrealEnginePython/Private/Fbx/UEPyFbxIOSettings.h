#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#if PLATFORM_LINUX
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wnull-dereference"
#endif
#endif

#include <fbxsdk.h>

struct ue_PyFbxIOSettings
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FbxIOSettings *fbx_io_settings;
};


void ue_python_init_fbx_io_settings(PyObject *);

ue_PyFbxIOSettings *py_ue_is_fbx_io_settings(PyObject *);
#endif
