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
PyObject *py_unreal_engine_get_assets_by_filter(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_set_fbx_import_option(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_discovered_plugins(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_enabled_plugins(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_plugin(PyObject *, PyObject *);

PyObject *py_unreal_engine_rename_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_duplicate_asset(PyObject *, PyObject *);
PyObject *py_unreal_engine_delete_asset(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_long_package_path(PyObject *, PyObject *);

PyObject *py_unreal_engine_create_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_compile_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_message_dialog_open(PyObject *, PyObject *);
PyObject *py_unreal_engine_reload_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_replace_blueprint(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_blueprint_from_actor(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_component_to_blueprint(PyObject *, PyObject *);

PyObject *py_unreal_engine_blueprint_add_member_variable(PyObject *, PyObject *);
PyObject *py_unreal_engine_blueprint_add_new_timeline(PyObject *, PyObject *);
PyObject *py_unreal_engine_blueprint_set_variable_visibility(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_play(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_on_asset_post_import(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_command_build(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_command_build_lighting(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_command_save_current_level(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_command_save_all_levels(PyObject *, PyObject *);

PyObject *py_unreal_engine_editor_save_all(PyObject *, PyObject *);

PyObject *py_unreal_engine_add_level_to_world(PyObject *, PyObject *);
PyObject *py_unreal_engine_move_selected_actors_to_level(PyObject *, PyObject *);


PyObject *py_ue_factory_create_new(ue_PyUObject *, PyObject *);

PyObject *py_unreal_engine_editor_take_high_res_screen_shots(PyObject *, PyObject *);

PyObject *py_unreal_engine_blueprint_add_function(PyObject *, PyObject *);
PyObject *py_unreal_engine_blueprint_add_ubergraph_page(PyObject *, PyObject *);

// efeng additional functions
PyObject *py_unreal_engine_create_material_instance(PyObject *, PyObject *);

PyObject *py_unreal_engine_allow_actor_script_execution_in_editor(PyObject *, PyObject *);

PyObject *py_unreal_engine_get_asset_referencers(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_asset_dependencies(PyObject *, PyObject *);

// transactions
PyObject *py_unreal_engine_begin_transaction(PyObject *, PyObject *);
PyObject *py_unreal_engine_cancel_transaction(PyObject *, PyObject *);
PyObject *py_unreal_engine_end_transaction(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_transaction_name(PyObject *, PyObject *);
PyObject *py_unreal_engine_is_transaction_active(PyObject *, PyObject *);
PyObject *py_unreal_engine_redo_transaction(PyObject *, PyObject *);
PyObject *py_unreal_engine_reset_transaction(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_undo(PyObject *, PyObject *);
PyObject *py_unreal_engine_editor_redo(PyObject *, PyObject *);
PyObject *py_unreal_engine_transactions(PyObject *, PyObject *);

#endif
