#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR

PyObject *py_unreal_engine_get_editor_world(PyObject *, PyObject * args);
PyObject *py_unreal_engine_editor_get_selected_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_deselect_actors(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_select_actor(PyObject *, PyObject *);
PyObject *py_unreal_engine_import_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_assets(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_selected_assets(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_assets_by_class(PyObject *, PyObject *);
PyObject *py_unreal_engine_set_fbx_import_option(PyObject *, PyObject *);

PyObject *py_unreal_engine_create_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_compile_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_message_dialog_open(PyObject *, PyObject *);
PyObject *py_unreal_engine_reload_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_replace_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_blueprint_from_actor(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_component_to_blueprint(PyObject *, PyObject *);

PyObject *py_unreal_engine_blueprint_add_member_variable(PyObject *, PyObject *);
PyObject *py_unreal_engine_blueprint_add_new_timeline(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_play(PyObject *, PyObject *);

#endif