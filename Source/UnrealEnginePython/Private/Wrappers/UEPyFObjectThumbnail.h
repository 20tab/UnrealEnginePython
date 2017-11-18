#pragma once
#include "UnrealEnginePython.h"

#include "Runtime/Core/Public/Misc/ObjectThumbnail.h"

struct ue_PyFObjectThumbnail {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FObjectThumbnail object_thumbnail;
};

void ue_python_init_fobject_thumbnail(PyObject *);

PyObject *py_ue_new_fobject_thumbnail(FObjectThumbnail);

ue_PyFObjectThumbnail *py_ue_is_fobject_thumbnail(PyObject *);
