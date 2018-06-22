#pragma once


#include "UEPyModule.h"


PyObject *py_ue_struct_add_variable(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_get_variables(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_remove_variable(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_move_variable_up(ue_PyUObject *, PyObject *);
PyObject *py_ue_struct_move_variable_down(ue_PyUObject *, PyObject *);