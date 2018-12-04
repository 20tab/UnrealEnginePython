#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FVector2D vec;
} ue_PyFVector2D;

extern PyTypeObject ue_PyFVector2DType;

PyObject *py_ue_new_fvector2d(FVector2D);
ue_PyFVector2D *py_ue_is_fvector2d(PyObject *);

void ue_python_init_fvector2d(PyObject *);

bool py_ue_vector2d_arg(PyObject *, FVector2D &);
