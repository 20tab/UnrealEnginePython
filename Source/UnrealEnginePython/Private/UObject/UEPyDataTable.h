#pragma once


#include "UEPyModule.h"


PyObject *py_ue_data_table_add_row(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_remove_row(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_rename_row(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_as_dict(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_as_json(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_find_row(ue_PyUObject *, PyObject *);
PyObject *py_ue_data_table_get_all_rows(ue_PyUObject *, PyObject *);