#pragma once



#include "UEPyModule.h"

PyObject *py_ue_line_trace_single_by_channel(ue_PyUObject *, PyObject *);
PyObject *py_ue_line_trace_multi_by_channel(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_hit_result_under_cursor(ue_PyUObject *, PyObject *);
PyObject *py_ue_draw_debug_line(ue_PyUObject *, PyObject *);