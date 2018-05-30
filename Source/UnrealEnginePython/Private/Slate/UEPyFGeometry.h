#pragma once

#include "UEPyModule.h"

#include "Runtime/SlateCore/Public/Layout/Geometry.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FGeometry geometry;
} ue_PyFGeometry;

void ue_python_init_fgeometry(PyObject *);

ue_PyFGeometry *py_ue_is_fgeometry(PyObject *);

PyObject *py_ue_new_fgeometry(FGeometry);
