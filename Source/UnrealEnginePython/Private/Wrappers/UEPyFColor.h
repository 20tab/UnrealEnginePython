#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FColor color;
} ue_PyFColor;

extern PyTypeObject ue_PyFColorType;

PyObject *py_ue_new_fcolor(FColor);
ue_PyFColor *py_ue_is_fcolor(PyObject *);

void ue_python_init_fcolor(PyObject *);

bool py_ue_color_arg(PyObject *, FColor &);

bool py_ue_get_fcolor(PyObject *, FColor &);
