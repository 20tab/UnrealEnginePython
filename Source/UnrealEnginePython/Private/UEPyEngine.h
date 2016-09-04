#pragma once



#include "UnrealEnginePython.h"

PyObject *py_unreal_engine_log(PyObject *, PyObject *);
PyObject *py_unreal_engine_log_warning(PyObject *, PyObject *);
PyObject *py_unreal_engine_log_error(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_on_screen_debug_message(PyObject *, PyObject *);
PyObject *py_unreal_engine_print_string(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_forward_vector(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_right_vector(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_up_vector(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_struct(PyObject *, PyObject *);

PyObject *py_unreal_engine_load_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_struct(PyObject *, PyObject *);

PyObject *py_unreal_engine_color_to_linear(PyObject *, PyObject *);
PyObject *py_unreal_engine_color_from_linear(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);


PyObject *py_unreal_engine_new_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_new_class(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_create_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_message_dialog_open(PyObject *, PyObject *);
#endif


