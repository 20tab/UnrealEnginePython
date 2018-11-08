#pragma once



#include "UEPyModule.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FLinearColor color;
} ue_PyFLinearColor;

extern PyTypeObject ue_PyFLinearColorType;

PyObject *py_ue_new_flinearcolor(FLinearColor);
ue_PyFLinearColor *py_ue_is_flinearcolor(PyObject *);

void ue_python_init_flinearcolor(PyObject *);

bool py_ue_linearcolor_arg(PyObject *, FLinearColor &);

bool py_ue_get_flinearcolor(PyObject *, FLinearColor &);
