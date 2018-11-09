#pragma once



#include "UEPyModule.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
    FBoxSphereBounds bounds;
} ue_PyFBoxSphereBounds;

extern PyTypeObject ue_PyFBoxSphereBoundsType;

PyObject *py_ue_new_fboxspherebounds(FBoxSphereBounds);
ue_PyFBoxSphereBounds *py_ue_is_fboxspherebounds(PyObject *);

void ue_python_init_fboxspherebounds(PyObject *);
bool py_ue_boxspherebounds_arg(PyObject *, FBoxSphereBounds &);
