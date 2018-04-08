#pragma once



#include "UEPyModule.h"


PyObject *py_ue_is_input_key_down(ue_PyUObject *, PyObject *);
PyObject *py_ue_was_input_key_just_pressed(ue_PyUObject *, PyObject *);
PyObject *py_ue_was_input_key_just_released(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_action_pressed(ue_PyUObject *, PyObject *);
PyObject *py_ue_is_action_released(ue_PyUObject *, PyObject *);
PyObject *py_ue_enable_input(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_input_axis(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_input_axis(ue_PyUObject *, PyObject *);
PyObject *py_ue_enable_mouse_over_events(ue_PyUObject *, PyObject *);
PyObject *py_ue_enable_click_events(ue_PyUObject *, PyObject *);
PyObject *py_ue_show_mouse_cursor(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_action(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_axis(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_key(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_pressed_key(ue_PyUObject *, PyObject *);
PyObject *py_ue_bind_released_key(ue_PyUObject *, PyObject *);

PyObject *py_ue_input_key(ue_PyUObject *, PyObject *);
PyObject *py_ue_input_axis(ue_PyUObject *, PyObject *);

PyObject *py_unreal_engine_get_engine_defined_action_mappings(PyObject *, PyObject *);