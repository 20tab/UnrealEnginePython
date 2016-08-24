#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_get_class(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_a(ue_PyUObject *, PyObject *);
PyObject *py_ue_call_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_find_function(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_name(ue_PyUObject *, PyObject * args);
PyObject *py_ue_get_full_name(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_properties(ue_PyUObject *, PyObject *);
PyObject *py_ue_call(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_property(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_rooted(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_to_root(ue_PyUObject *, PyObject *);
PyObject *py_ue_remove_from_root(ue_PyUObject *, PyObject *);