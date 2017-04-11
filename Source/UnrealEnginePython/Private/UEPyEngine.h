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

PyObject *py_unreal_engine_string_to_guid(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);

PyObject *py_unreal_engine_all_classes(PyObject *, PyObject *);


PyObject *py_unreal_engine_new_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_new_class(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_mutable_default(PyObject *, PyObject *);


PyObject *py_unreal_engine_create_and_dispatch_when_ready(PyObject *, PyObject *);

PyObject *py_unreal_engine_convert_relative_path_to_full(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_viewport_screenshot(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_viewport_size(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_editor_get_active_viewport_screenshot(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_get_pie_viewport_screenshot(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_get_active_viewport_size(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_get_pie_viewport_size(PyObject *, PyObject *);
#endif



