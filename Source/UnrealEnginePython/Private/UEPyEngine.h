#pragma once



#include "UEPyModule.h"

PyObject *py_unreal_engine_log(PyObject *, PyObject *);
PyObject *py_unreal_engine_log_warning(PyObject *, PyObject *);
PyObject *py_unreal_engine_log_error(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_on_screen_debug_message(PyObject *, PyObject *);
PyObject *py_unreal_engine_print_string(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_forward_vector(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_right_vector(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_up_vector(PyObject *, PyObject *);

PyObject *py_unreal_engine_clipboard_copy(PyObject *, PyObject *);
PyObject *py_unreal_engine_clipboard_paste(PyObject *, PyObject *);

PyObject *py_unreal_engine_set_random_seed(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_game_viewport_size(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_resolution(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_content_dir(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_game_saved_dir(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_game_user_developer_dir(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_struct(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_enum(PyObject *, PyObject *);

PyObject *py_unreal_engine_load_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_struct(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_enum(PyObject *, PyObject *);
PyObject *py_unreal_engine_load_package(PyObject *, PyObject *);
#if WITH_EDITOR
PyObject *py_unreal_engine_unload_package(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_package_filename(PyObject *, PyObject *);
#endif

PyObject *py_unreal_engine_string_to_guid(PyObject *, PyObject *);
PyObject *py_unreal_engine_new_guid(PyObject *, PyObject *);
PyObject *py_unreal_engine_guid_to_string(PyObject *, PyObject *);

PyObject *py_unreal_engine_engine_tick(PyObject *, PyObject *);
PyObject *py_unreal_engine_slate_tick(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_delta_time(PyObject *, PyObject *);

PyObject *py_unreal_engine_tick_rendering_tickables(PyObject *, PyObject *);

PyObject *py_unreal_engine_all_classes(PyObject *, PyObject *);
PyObject *py_unreal_engine_all_worlds(PyObject *, PyObject *);
PyObject *py_unreal_engine_tobject_iterator(PyObject *, PyObject *);

PyObject *py_unreal_engine_all_worlds(PyObject *, PyObject *);
PyObject *py_unreal_engine_tobject_iterator(PyObject *, PyObject *);


PyObject *py_unreal_engine_new_object(PyObject *, PyObject *);
PyObject *py_unreal_engine_new_class(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_mutable_default(PyObject *, PyObject *);


PyObject *py_unreal_engine_create_and_dispatch_when_ready(PyObject *, PyObject *);

PyObject *py_unreal_engine_convert_relative_path_to_full(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_viewport_screenshot(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_viewport_size(PyObject *, PyObject *);

PyObject *py_unreal_engine_create_world(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_package(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_or_create_package(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_transient_package(PyObject *, PyObject *);

PyObject *py_unreal_engine_object_path_to_package_name(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_path(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_base_filename(PyObject *, PyObject *);

PyObject *py_unreal_engine_open_file_dialog(PyObject *, PyObject *);
PyObject *py_unreal_engine_open_font_dialog(PyObject *, PyObject *);
PyObject *py_unreal_engine_open_directory_dialog(PyObject *, PyObject *);
PyObject *py_unreal_engine_save_file_dialog(PyObject *, PyObject *);

PyObject *py_unreal_engine_copy_properties_for_unrelated_objects(PyObject *, PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_editor_get_active_viewport_screenshot(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_get_pie_viewport_screenshot(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_get_active_viewport_size(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_get_pie_viewport_size(PyObject *, PyObject *);
#endif


#if PLATFORM_MAC
PyObject *py_unreal_engine_main_thread_call(PyObject *, PyObject *);
#endif
