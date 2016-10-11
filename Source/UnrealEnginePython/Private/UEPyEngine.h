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

PyObject *py_unreal_engine_get_content_dir(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_struct(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_enum(PyObject *, PyObject *);

PyObject *py_unreal_engine_load_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_struct(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_enum(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);

PyObject *py_unreal_engine_classes(PyObject *, PyObject *);


PyObject *py_unreal_engine_new_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_new_class(PyObject *, PyObject *);


PyObject *py_unreal_engine_create_and_dispatch_when_ready(PyObject *, PyObject *);

PyObject *py_unreal_engine_add_ticker(PyObject *, PyObject *);



