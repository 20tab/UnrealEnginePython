#pragma once


#include "UEPyModule.h"


PyObject *py_ue_struct_add_variable(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_get_variables(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_remove_variable(ue_PyUObject *, PyObject *);
#if ENGINE_MAJOR_VERSION == 5
PyObject *py_ue_struct_move_variable_above(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_move_variable_below(ue_PyUObject *, PyObject *);
#else
PyObject *py_ue_struct_move_variable_up(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_move_variable_down(ue_PyUObject *, PyObject *);
#endif