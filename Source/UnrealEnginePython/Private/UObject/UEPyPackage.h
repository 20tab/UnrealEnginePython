#pragma once



#include "UEPyModule.h"

PyObject *py_ue_package_is_dirty(ue_PyUObject *, PyObject *);
PyObject *py_ue_package_get_filename(ue_PyUObject *, PyObject *);
PyObject *py_ue_package_make_unique_object_name(ue_PyUObject *, PyObject *);
