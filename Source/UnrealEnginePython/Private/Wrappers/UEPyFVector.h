#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FVector vec;
} ue_PyFVector;

extern PyTypeObject ue_PyFVectorType;

PyObject *py_ue_new_fvector(FVector);
ue_PyFVector *py_ue_is_fvector(PyObject *);

void ue_python_init_fvector(PyObject *);

bool py_ue_vector_arg(PyObject *, FVector &);
