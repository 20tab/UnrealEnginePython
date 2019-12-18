#pragma once



#include "UEPyModule.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FTransform transform;
} ue_PyFTransform;

extern PyTypeObject ue_PyFTransformType;

PyObject *py_ue_new_ftransform(FTransform);
ue_PyFTransform *py_ue_is_ftransform(PyObject *);

void ue_python_init_ftransform(PyObject *);
bool py_ue_transform_arg(PyObject *, FTransform &);
