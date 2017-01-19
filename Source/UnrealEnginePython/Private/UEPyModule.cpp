#include "UnrealEnginePythonPrivatePCH.h"



#include "UEPyEngine.h"
#include "UEPyObject.h"
#include "UEPyActor.h"
#include "UEPyTransform.h"
#include "UEPyPlayer.h"
#include "UEPyInput.h"
#include "UEPyWorld.h"
#include "UEPyNavigation.h"
#include "UEPySpline.h"
#include "UEPyMovements.h"
#include "UEPyAttaching.h"
#include "UEPySkeletal.h"
#include "UEPyTraceAndSweep.h"
#include "UEPyTimer.h"
#include "UEPyTicker.h"
#include "UEPyPhysics.h"
#include "UEPyAudio.h"
#include "UEPySequencer.h"
#include "UEPySlate.h"
#include "UEPyPackage.h"
#include "UEPyAssetUserData.h"
#include "UEPyTexture.h"
#include "UEPyMaterial.h"
#if WITH_EDITOR
#include "UEPyEditor.h"
#include "UEPyEdGraph.h"
#endif


#include "PythonDelegate.h"
#include "PythonFunction.h"
#include "PythonClass.h"


DEFINE_LOG_CATEGORY(LogPython);


PyDoc_STRVAR(unreal_engine_py_doc, "Unreal Engine Python module.");

#if PY_MAJOR_VERSION >= 3
static PyModuleDef unreal_engine_module = {
	PyModuleDef_HEAD_INIT,
	"unreal_engine",
	unreal_engine_py_doc,
	-1,
	NULL,
};

static PyObject *init_unreal_engine(void) {
	return PyModule_Create(&unreal_engine_module);
}
#endif

std::map<UObject *, ue_PyUObject *> ue_python_gc;


static PyObject *py_unreal_engine_py_gc(PyObject * self, PyObject * args) {
	std::list<UObject *> broken_list;
	for (auto it : ue_python_gc) {
#if UEPY_MEMORY_DEBUG
		UE_LOG(LogPython, Warning, TEXT("Checking for UObject at %p"), it.first);
#endif
		UObject *u_obj = it.first;
		if (!u_obj || !u_obj->IsValidLowLevel() || u_obj->IsPendingKillOrUnreachable()) {
#if UEPY_MEMORY_DEBUG
			UE_LOG(LogPython, Warning, TEXT("Removing UObject at %p (refcnt: %d)"), it.first, it.second->ob_base.ob_refcnt);
#endif
			broken_list.push_back(u_obj);
		}
	}
	for (UObject *u_obj : broken_list) {
		ue_PyUObject *py_obj = ue_python_gc.at(u_obj);
		Py_DECREF(py_obj);
	}

	return PyLong_FromLong(broken_list.size());

}

static PyObject *py_unreal_engine_exec(PyObject * self, PyObject * args) {
	char *filename = nullptr;
	if (!PyArg_ParseTuple(args, "s:exec", &filename)) {
		return NULL;
	}
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.RunFile(filename);
	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_get_py_proxy(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	if (self->py_proxy) {
		Py_INCREF(self->py_proxy);
		return (PyObject *)self->py_proxy;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef unreal_engine_methods[] = {
	{ "log", py_unreal_engine_log, METH_VARARGS, "" },
	{ "log_warning", py_unreal_engine_log_warning, METH_VARARGS, "" },
	{ "log_error", py_unreal_engine_log_error, METH_VARARGS, "" },

	{ "add_on_screen_debug_message", py_unreal_engine_add_on_screen_debug_message, METH_VARARGS, "" },
	{ "print_string", py_unreal_engine_print_string, METH_VARARGS, "" },

	{ "find_class", py_unreal_engine_find_class, METH_VARARGS, "" },
	{ "find_struct", py_unreal_engine_find_struct, METH_VARARGS, "" },
	{ "find_enum", py_unreal_engine_find_enum, METH_VARARGS, "" },

	{ "load_class", py_unreal_engine_load_class, METH_VARARGS, "" },
	{ "load_struct", py_unreal_engine_load_struct, METH_VARARGS, "" },
	{ "load_enum", py_unreal_engine_load_enum, METH_VARARGS, "" },

	{ "find_object", py_unreal_engine_find_object, METH_VARARGS, "" },
	{ "load_object", py_unreal_engine_load_object, METH_VARARGS, "" },

	{ "get_forward_vector", py_unreal_engine_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", py_unreal_engine_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", py_unreal_engine_get_right_vector, METH_VARARGS, "" },

	{ "get_content_dir", py_unreal_engine_get_content_dir, METH_VARARGS, "" },
	{ "convert_relative_path_to_full", py_unreal_engine_convert_relative_path_to_full, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_editor_world", py_unreal_engine_get_editor_world, METH_VARARGS, "" },
	{ "editor_get_selected_actors", py_unreal_engine_editor_get_selected_actors, METH_VARARGS, "" },
	{ "editor_select_actor", py_unreal_engine_editor_select_actor, METH_VARARGS, "" },
	{ "editor_deselect_actors", py_unreal_engine_editor_deselect_actors, METH_VARARGS, "" },
	{ "import_asset", py_unreal_engine_import_asset, METH_VARARGS, "" },
	{ "get_asset", py_unreal_engine_get_asset, METH_VARARGS, "" },
	{ "get_assets", py_unreal_engine_get_assets, METH_VARARGS, "" },
	{ "get_selected_assets", py_unreal_engine_get_selected_assets, METH_VARARGS, "" },
	{ "get_assets_by_class", py_unreal_engine_get_assets_by_class, METH_VARARGS, "" },

	{ "rename_asset", py_unreal_engine_rename_asset, METH_VARARGS, "" },
	{ "duplicate_asset", py_unreal_engine_duplicate_asset, METH_VARARGS, "" },
	{ "delete_asset", py_unreal_engine_delete_asset, METH_VARARGS, "" },

	{ "get_long_package_path", py_unreal_engine_get_long_package_path, METH_VARARGS, "" },

	{ "editor_command_build", py_unreal_engine_editor_command_build, METH_VARARGS, "" },
	{ "editor_command_build_lighting", py_unreal_engine_editor_command_build_lighting, METH_VARARGS, "" },
	{ "editor_command_save_current_level", py_unreal_engine_editor_command_save_current_level, METH_VARARGS, "" },
	{ "editor_command_save_all_levels", py_unreal_engine_editor_command_save_all_levels, METH_VARARGS, "" },

	{ "editor_save_all", py_unreal_engine_editor_save_all, METH_VARARGS, "" },
#pragma warning(suppress: 4191)
	{ "get_assets_by_filter", (PyCFunction)py_unreal_engine_get_assets_by_filter, METH_VARARGS | METH_KEYWORDS, "" },
	{ "create_blueprint", py_unreal_engine_create_blueprint, METH_VARARGS, "" },
	{ "create_blueprint_from_actor", py_unreal_engine_create_blueprint_from_actor, METH_VARARGS, "" },
	{ "replace_blueprint", py_unreal_engine_replace_blueprint, METH_VARARGS, "" },
	{ "reload_blueprint", py_unreal_engine_reload_blueprint, METH_VARARGS, "" },
	{ "compile_blueprint", py_unreal_engine_compile_blueprint, METH_VARARGS, "" },
	{ "blueprint_add_member_variable", py_unreal_engine_blueprint_add_member_variable, METH_VARARGS, "" },
	{ "blueprint_add_new_timeline", py_unreal_engine_blueprint_add_new_timeline, METH_VARARGS, "" },
	{ "blueprint_set_variable_visibility", py_unreal_engine_blueprint_set_variable_visibility, METH_VARARGS, "" },
	{ "blueprint_add_function", py_unreal_engine_blueprint_add_function, METH_VARARGS, "" },
	{ "blueprint_add_ubergraph_page", py_unreal_engine_blueprint_add_ubergraph_page, METH_VARARGS, "" },
	{ "add_component_to_blueprint", py_unreal_engine_add_component_to_blueprint, METH_VARARGS, "" },
	{ "create_material_instance", py_unreal_engine_create_material_instance, METH_VARARGS, "" },
	{ "message_dialog_open", py_unreal_engine_message_dialog_open, METH_VARARGS, "" },
	{ "set_fbx_import_option", py_unreal_engine_set_fbx_import_option, METH_VARARGS, "" },

	{ "editor_play", py_unreal_engine_editor_play, METH_VARARGS, "" },

	{ "add_menu_extension", py_unreal_engine_add_menu_extension, METH_VARARGS, "" },
	{ "add_nomad_tab", py_unreal_engine_add_nomad_tab, METH_VARARGS, "" },

	{ "slate_text_block", py_unreal_engine_slate_text_block, METH_VARARGS, "" },
	{ "slate_box", py_unreal_engine_slate_box, METH_VARARGS, "" },
	{ "slate_window", py_unreal_engine_slate_window, METH_VARARGS, "" },
	{ "slate_button", py_unreal_engine_slate_button, METH_VARARGS, "" },

	{ "get_editor_window", py_unreal_engine_get_editor_window, METH_VARARGS, "" },

	{ "add_level_to_world", py_unreal_engine_add_level_to_world, METH_VARARGS, "" },
	{ "move_selected_actors_to_level", py_unreal_engine_move_selected_actors_to_level, METH_VARARGS, "" },

	{ "editor_on_asset_post_import", py_unreal_engine_editor_on_asset_post_import, METH_VARARGS, "" },
#endif

	{ "new_object", py_unreal_engine_new_object, METH_VARARGS, "" },

	{ "classes", (PyCFunction)py_unreal_engine_classes, METH_VARARGS, "" },


	{ "new_class", py_unreal_engine_new_class, METH_VARARGS, "" },


	{ "create_and_dispatch_when_ready", py_unreal_engine_create_and_dispatch_when_ready, METH_VARARGS, "" },

	{ "add_ticker", py_unreal_engine_add_ticker, METH_VARARGS, "" },
	{ "remove_ticker", py_unreal_engine_remove_ticker, METH_VARARGS, "" },

	{ "py_gc", py_unreal_engine_py_gc, METH_VARARGS, "" },
	// exec is a reserved keyword in python2
#if PY_MAJOR_VERSION >= 3
	{ "exec", py_unreal_engine_exec, METH_VARARGS, "" },
#endif
	{ "py_exec", py_unreal_engine_exec, METH_VARARGS, "" },

	{ "get_engine_defined_action_mappings", py_unreal_engine_get_engine_defined_action_mappings, METH_VARARGS, "" },

	{ "get_viewport_screenshot", py_unreal_engine_get_viewport_screenshot, METH_VARARGS, "" },
	{ "get_viewport_size", py_unreal_engine_get_viewport_size, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "editor_get_active_viewport_screenshot", py_unreal_engine_editor_get_active_viewport_screenshot, METH_VARARGS, "" },
	{ "editor_get_pie_viewport_screenshot", py_unreal_engine_editor_get_pie_viewport_screenshot, METH_VARARGS, "" },

	{ "editor_get_active_viewport_size", py_unreal_engine_editor_get_active_viewport_size, METH_VARARGS, "" },
	{ "editor_get_pie_viewport_size", py_unreal_engine_editor_get_pie_viewport_size, METH_VARARGS, "" },

	{ "editor_take_high_res_screen_shots", py_unreal_engine_editor_take_high_res_screen_shots, METH_VARARGS, "" },
#endif


	{ NULL, NULL },
};


static PyMethodDef ue_PyUObject_methods[] = {

	// Transform

	{ "get_actor_location", (PyCFunction)py_ue_get_actor_location, METH_VARARGS, "" },
	{ "get_actor_rotation", (PyCFunction)py_ue_get_actor_rotation, METH_VARARGS, "" },
	{ "get_actor_scale", (PyCFunction)py_ue_get_actor_scale, METH_VARARGS, "" },

	{ "get_actor_forward", (PyCFunction)py_ue_get_actor_forward, METH_VARARGS, "" },
	{ "get_actor_right", (PyCFunction)py_ue_get_actor_right, METH_VARARGS, "" },
	{ "get_actor_up", (PyCFunction)py_ue_get_actor_up, METH_VARARGS, "" },


	{ "set_actor_location", (PyCFunction)py_ue_set_actor_location, METH_VARARGS, "" },
	{ "set_actor_rotation", (PyCFunction)py_ue_set_actor_rotation, METH_VARARGS, "" },
	{ "set_actor_scale", (PyCFunction)py_ue_set_actor_scale, METH_VARARGS, "" },


	{ "get_world_location", (PyCFunction)py_ue_get_world_location, METH_VARARGS, "" },
	{ "get_world_rotation", (PyCFunction)py_ue_get_world_rotation, METH_VARARGS, "" },
	{ "get_world_scale", (PyCFunction)py_ue_get_world_scale, METH_VARARGS, "" },
	{ "get_relative_location", (PyCFunction)py_ue_get_relative_location, METH_VARARGS, "" },
	{ "get_relative_rotation", (PyCFunction)py_ue_get_relative_rotation, METH_VARARGS, "" },
	{ "get_relative_scale", (PyCFunction)py_ue_get_relative_scale, METH_VARARGS, "" },

	{ "set_world_location", (PyCFunction)py_ue_set_world_location, METH_VARARGS, "" },
	{ "set_world_rotation", (PyCFunction)py_ue_set_world_rotation, METH_VARARGS, "" },
	{ "set_world_scale", (PyCFunction)py_ue_set_world_scale, METH_VARARGS, "" },
	{ "set_relative_location", (PyCFunction)py_ue_set_relative_location, METH_VARARGS, "" },
	{ "set_relative_rotation", (PyCFunction)py_ue_set_relative_rotation, METH_VARARGS, "" },
	{ "set_relative_scale", (PyCFunction)py_ue_set_relative_scale, METH_VARARGS, "" },

	{ "get_forward_vector", (PyCFunction)py_ue_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", (PyCFunction)py_ue_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", (PyCFunction)py_ue_get_right_vector, METH_VARARGS, "" },

	// UObject

	{ "get_property", (PyCFunction)py_ue_get_property, METH_VARARGS, "" },
	{ "set_property", (PyCFunction)py_ue_set_property, METH_VARARGS, "" },
	{ "properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },


	{ "functions", (PyCFunction)py_ue_functions, METH_VARARGS, "" },

	{ "is_a", (PyCFunction)py_ue_is_a, METH_VARARGS, "" },
	{ "is_child_of", (PyCFunction)py_ue_is_child_of, METH_VARARGS, "" },

	{ "call", (PyCFunction)py_ue_call, METH_VARARGS, "" },
	{ "get_owner", (PyCFunction)py_ue_get_owner, METH_VARARGS, "" },

	{ "get_outer", (PyCFunction)py_ue_get_outer, METH_VARARGS, "" },
	{ "get_outermost", (PyCFunction)py_ue_get_outermost, METH_VARARGS, "" },

	{ "get_super_class", (PyCFunction)py_ue_get_super_class, METH_VARARGS, "" },


	{ "get_name", (PyCFunction)py_ue_get_name, METH_VARARGS, "" },
	{ "get_path_name", (PyCFunction)py_ue_get_path_name, METH_VARARGS, "" },
	{ "get_full_name", (PyCFunction)py_ue_get_full_name, METH_VARARGS, "" },

	{ "set_name", (PyCFunction)py_ue_set_name, METH_VARARGS, "" },

	{ "bind_event", (PyCFunction)py_ue_bind_event, METH_VARARGS, "" },

	{ "get_py_proxy", (PyCFunction)py_ue_get_py_proxy, METH_VARARGS, "" },

	{ "post_edit_change", (PyCFunction)py_ue_post_edit_change, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_actor_label", (PyCFunction)py_ue_get_actor_label, METH_VARARGS, "" },
	{ "set_actor_label", (PyCFunction)py_ue_set_actor_label, METH_VARARGS, "" },

	{ "find_actor_by_label", (PyCFunction)py_ue_find_actor_by_label, METH_VARARGS, "" },
	{ "save_package", (PyCFunction)py_ue_save_package, METH_VARARGS, "" },
	{ "asset_can_reimport", (PyCFunction)py_ue_asset_can_reimport, METH_VARARGS, "" },
	{ "asset_reimport", (PyCFunction)py_ue_asset_reimport, METH_VARARGS, "" },

	{ "factory_create_new", (PyCFunction)py_ue_factory_create_new, METH_VARARGS, "" },

	{ "graph_add_node_call_function", (PyCFunction)py_ue_graph_add_node_call_function, METH_VARARGS, "" },
	{ "graph_add_node_custom_event", (PyCFunction)py_ue_graph_add_node_custom_event, METH_VARARGS, "" },
	{ "graph_add_node_variable_get", (PyCFunction)py_ue_graph_add_node_variable_get, METH_VARARGS, "" },
	{ "graph_add_node_variable_set", (PyCFunction)py_ue_graph_add_node_variable_set, METH_VARARGS, "" },

	{ "graph_add_node", (PyCFunction)py_ue_graph_add_node, METH_VARARGS, "" },
	{ "graph_add_node_event", (PyCFunction)py_ue_graph_add_node_event, METH_VARARGS, "" },
	{ "graph_get_good_place_for_new_node", (PyCFunction)py_ue_graph_get_good_place_for_new_node, METH_VARARGS, "" },

	{ "node_pins", (PyCFunction)py_ue_node_pins, METH_VARARGS, "" },
	{ "node_find_pin", (PyCFunction)py_ue_node_find_pin, METH_VARARGS, "" },
#endif

	{ "is_rooted", (PyCFunction)py_ue_is_rooted, METH_VARARGS, "" },
	{ "add_to_root", (PyCFunction)py_ue_add_to_root, METH_VARARGS, "" },
	{ "remove_from_root", (PyCFunction)py_ue_remove_from_root, METH_VARARGS, "" },

	{ "find_function", (PyCFunction)py_ue_find_function, METH_VARARGS, "" },

#pragma warning(suppress: 4191)
	{ "call_function", (PyCFunction)py_ue_call_function, METH_VARARGS | METH_KEYWORDS, "" },


	{ "all_objects", (PyCFunction)py_ue_all_objects, METH_VARARGS, "" },
	{ "all_actors", (PyCFunction)py_ue_all_actors, METH_VARARGS, "" },


	// Package
	{ "package_get_filename", (PyCFunction)py_ue_package_get_filename, METH_VARARGS, "" },
	{ "package_is_dirty", (PyCFunction)py_ue_package_is_dirty, METH_VARARGS, "" },

#if WITH_EDITOR
	// AssetUserData
	{ "asset_import_data", (PyCFunction)py_ue_asset_import_data, METH_VARARGS, "" },

	// AnimSequence
	{ "anim_sequence_get_skeleton", (PyCFunction)py_ue_anim_sequence_get_skeleton, METH_VARARGS, "" },
#endif

	// Input

	{ "get_input_axis", (PyCFunction)py_ue_get_input_axis, METH_VARARGS, "" },
	{ "bind_input_axis", (PyCFunction)py_ue_bind_input_axis, METH_VARARGS, "" },
	{ "enable_input", (PyCFunction)py_ue_enable_input, METH_VARARGS, "" },
	{ "show_mouse_cursor", (PyCFunction)py_ue_show_mouse_cursor, METH_VARARGS, "" },
	{ "enable_click_events", (PyCFunction)py_ue_enable_click_events, METH_VARARGS, "" },
	{ "enable_mouse_over_events", (PyCFunction)py_ue_enable_mouse_over_events, METH_VARARGS, "" },
	{ "was_input_key_just_pressed", (PyCFunction)py_ue_was_input_key_just_pressed, METH_VARARGS, "" },
	{ "was_input_key_just_released", (PyCFunction)py_ue_was_input_key_just_released, METH_VARARGS, "" },
	{ "is_action_pressed", (PyCFunction)py_ue_is_action_pressed, METH_VARARGS, "" },
	{ "is_action_released", (PyCFunction)py_ue_is_action_released, METH_VARARGS, "" },
	{ "is_input_key_down", (PyCFunction)py_ue_is_input_key_down, METH_VARARGS, "" },
	{ "bind_action", (PyCFunction)py_ue_bind_action, METH_VARARGS, "" },
	{ "bind_axis", (PyCFunction)py_ue_bind_axis, METH_VARARGS, "" },
	{ "bind_key", (PyCFunction)py_ue_bind_key, METH_VARARGS, "" },
	{ "bind_pressed_key", (PyCFunction)py_ue_bind_pressed_key, METH_VARARGS, "" },
	{ "bind_released_key", (PyCFunction)py_ue_bind_released_key, METH_VARARGS, "" },


	// Movements

	{ "add_controller_pitch_input", (PyCFunction)py_ue_add_controller_pitch_input, METH_VARARGS, "" },
	{ "add_controller_yaw_input", (PyCFunction)py_ue_add_controller_yaw_input, METH_VARARGS, "" },
	{ "get_control_rotation", (PyCFunction)py_ue_get_control_rotation, METH_VARARGS, "" },
	{ "add_movement_input", (PyCFunction)py_ue_add_movement_input, METH_VARARGS, "" },

	{ "jump", (PyCFunction)py_ue_jump, METH_VARARGS, "" },
	{ "stop_jumping", (PyCFunction)py_ue_stop_jumping, METH_VARARGS, "" },
	{ "crouch", (PyCFunction)py_ue_crouch, METH_VARARGS, "" },
	{ "uncrouch", (PyCFunction)py_ue_uncrouch, METH_VARARGS, "" },
	{ "launch", (PyCFunction)py_ue_launch, METH_VARARGS, "" },
	{ "is_jumping", (PyCFunction)py_ue_is_jumping, METH_VARARGS, "" },
	{ "is_crouched", (PyCFunction)py_ue_is_crouched, METH_VARARGS, "" },
	{ "is_falling", (PyCFunction)py_ue_is_falling, METH_VARARGS, "" },
	{ "is_flying", (PyCFunction)py_ue_is_falling, METH_VARARGS, "" },
	{ "can_jump", (PyCFunction)py_ue_can_jump, METH_VARARGS, "" },
	{ "can_crouch", (PyCFunction)py_ue_can_crouch, METH_VARARGS, "" },


	{ "get_class", (PyCFunction)py_ue_get_class, METH_VARARGS, "" },
	{ "get_actor_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "get_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },

	{ "component_is_registered", (PyCFunction)py_ue_component_is_registered, METH_VARARGS, "" },
	{ "register_component", (PyCFunction)py_ue_register_component, METH_VARARGS, "" },



	{ "actor_create_default_subobject", (PyCFunction)py_ue_actor_create_default_subobject, METH_VARARGS, "" },

	{ "broadcast", (PyCFunction)py_ue_broadcast, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_metadata", (PyCFunction)py_ue_get_metadata, METH_VARARGS, "" },
	{ "set_metadata", (PyCFunction)py_ue_set_metadata, METH_VARARGS, "" },
	{ "has_metadata", (PyCFunction)py_ue_has_metadata, METH_VARARGS, "" },
#endif


	{ "quit_game", (PyCFunction)py_ue_quit_game, METH_VARARGS, "" },
	{ "play", (PyCFunction)py_ue_play, METH_VARARGS, "" },

	{ "simple_move_to_location", (PyCFunction)py_ue_simple_move_to_location, METH_VARARGS, "" },

	{ "actor_has_component_of_type", (PyCFunction)py_ue_actor_has_component_of_type, METH_VARARGS, "" },

	{ "actor_destroy", (PyCFunction)py_ue_actor_destroy, METH_VARARGS, "" },
	{ "actor_spawn", (PyCFunction)py_ue_actor_spawn, METH_VARARGS, "" },
	{ "actor_has_tag", (PyCFunction)py_ue_actor_has_tag, METH_VARARGS, "" },
	{ "get_actor_bounds", (PyCFunction)py_ue_get_actor_bounds, METH_VARARGS, "" },

	{ "line_trace_single_by_channel", (PyCFunction)py_ue_line_trace_single_by_channel, METH_VARARGS, "" },
	{ "line_trace_multi_by_channel", (PyCFunction)py_ue_line_trace_multi_by_channel, METH_VARARGS, "" },
	{ "get_hit_result_under_cursor", (PyCFunction)py_ue_get_hit_result_under_cursor, METH_VARARGS, "" },
	{ "draw_debug_line", (PyCFunction)py_ue_draw_debug_line, METH_VARARGS, "" },

	{ "destructible_apply_damage", (PyCFunction)py_ue_destructible_apply_damage, METH_VARARGS, "" },

	{ "set_view_target", (PyCFunction)py_ue_set_view_target, METH_VARARGS, "" },
	{ "get_world_delta_seconds", (PyCFunction)py_ue_get_world_delta_seconds, METH_VARARGS, "" },

	{ "get_levels", (PyCFunction)py_ue_get_levels, METH_VARARGS, "" },

	{ "add_actor_component", (PyCFunction)py_ue_add_actor_component, METH_VARARGS, "" },
	{ "add_actor_root_component", (PyCFunction)py_ue_add_actor_root_component, METH_VARARGS, "" },
	{ "get_actor_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },

	{ "set_simulate_physics", (PyCFunction)py_ue_set_simulate_physics, METH_VARARGS, "" },
	{ "add_impulse", (PyCFunction)py_ue_add_impulse, METH_VARARGS, "" },
	{ "add_angular_impulse", (PyCFunction)py_ue_add_angular_impulse, METH_VARARGS, "" },
	{ "add_force", (PyCFunction)py_ue_add_force, METH_VARARGS, "" },
	{ "add_torque", (PyCFunction)py_ue_add_torque, METH_VARARGS, "" },
	{ "set_physics_linear_velocity", (PyCFunction)py_ue_set_physics_linear_velocity, METH_VARARGS, "" },
	{ "get_physics_linear_velocity", (PyCFunction)py_ue_get_physics_linear_velocity, METH_VARARGS, "" },
	{ "set_physics_angular_velocity", (PyCFunction)py_ue_set_physics_angular_velocity, METH_VARARGS, "" },
	{ "get_physics_angular_velocity", (PyCFunction)py_ue_get_physics_angular_velocity, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_get_world, METH_VARARGS, "" },
	{ "has_world", (PyCFunction)py_ue_has_world, METH_VARARGS, "" },

	{ "get_world_location_at_distance_along_spline", (PyCFunction)py_ue_get_world_location_at_distance_along_spline, METH_VARARGS, "" },
	{ "get_spline_length", (PyCFunction)py_ue_get_spline_length, METH_VARARGS, "" },

	{ "get_actor_velocity", (PyCFunction)py_ue_get_actor_velocity, METH_VARARGS, "" },

	{ "play_sound_at_location", (PyCFunction)py_ue_play_sound_at_location, METH_VARARGS, "" },

	{ "world_tick", (PyCFunction)py_ue_world_tick, METH_VARARGS, "" },

	{ "conditional_begin_destroy", (PyCFunction)py_ue_conditional_begin_destroy, METH_VARARGS, "" },

	{ "create_player", (PyCFunction)py_ue_create_player, METH_VARARGS, "" },
	{ "get_num_players", (PyCFunction)py_ue_get_num_players, METH_VARARGS, "" },
	{ "get_num_spectators", (PyCFunction)py_ue_get_num_spectators, METH_VARARGS, "" },
	{ "get_player_controller", (PyCFunction)py_ue_get_player_controller, METH_VARARGS, "" },

	{ "get_overlapping_actors", (PyCFunction)py_ue_get_overlapping_actors, METH_VARARGS, "" },


	// Attaching

	{ "get_socket_location", (PyCFunction)py_ue_get_socket_location, METH_VARARGS, "" },
	{ "get_socket_rotation", (PyCFunction)py_ue_get_socket_rotation, METH_VARARGS, "" },
	{ "get_socket_transform", (PyCFunction)py_ue_get_socket_transform, METH_VARARGS, "" },
	{ "get_socket_world_transform", (PyCFunction)py_ue_get_socket_world_transform, METH_VARARGS, "" },
	{ "get_socket_actor_transform", (PyCFunction)py_ue_get_socket_actor_transform, METH_VARARGS, "" },
	{ "get_attached_actors", (PyCFunction)py_ue_get_attached_actors, METH_VARARGS, "" },
	{ "get_all_child_actors", (PyCFunction)py_ue_get_all_child_actors, METH_VARARGS, "" },
	{ "attach_to_actor", (PyCFunction)py_ue_attach_to_actor, METH_VARARGS, "" },
	{ "attach_to_component", (PyCFunction)py_ue_attach_to_component, METH_VARARGS, "" },

	// Skeletal
	{ "get_anim_instance", (PyCFunction)py_ue_get_anim_instance, METH_VARARGS, "" },

	// Timer
	{ "set_timer", (PyCFunction)py_ue_set_timer, METH_VARARGS, "" },

	// Texture
	{ "texture_get_data", (PyCFunction)py_ue_texture_get_data, METH_VARARGS, "" },
	{ "texture_set_data", (PyCFunction)py_ue_texture_set_data, METH_VARARGS, "" },

	// Sequencer
	{ "sequencer_master_tracks", (PyCFunction)py_ue_sequencer_master_tracks, METH_VARARGS, "" },
	{ "sequencer_possessable_tracks", (PyCFunction)py_ue_sequencer_possessable_tracks, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "sequencer_folders", (PyCFunction)py_ue_sequencer_folders, METH_VARARGS, "" },
	{ "sequencer_set_display_name", (PyCFunction)py_ue_sequencer_set_display_name, METH_VARARGS, "" },
	{ "sequencer_get_display_name", (PyCFunction)py_ue_sequencer_get_display_name, METH_VARARGS, "" },
#endif
	{ "sequencer_sections", (PyCFunction)py_ue_sequencer_sections, METH_VARARGS, "" },
	{ "sequencer_track_sections", (PyCFunction)py_ue_sequencer_track_sections, METH_VARARGS, "" },
	{ "sequencer_possessables", (PyCFunction)py_ue_sequencer_possessables, METH_VARARGS, "" },
	{ "sequencer_find_possessable", (PyCFunction)py_ue_sequencer_find_possessable, METH_VARARGS, "" },
	{ "sequencer_add_master_track", (PyCFunction)py_ue_sequencer_add_master_track, METH_VARARGS, "" },

	// Material
	{ "set_material", (PyCFunction)py_ue_set_material, METH_VARARGS, "" },
	{ "set_material_scalar_parameter", (PyCFunction)py_ue_set_material_scalar_parameter, METH_VARARGS, "" },
	{ "set_material_vector_parameter", (PyCFunction)py_ue_set_material_vector_parameter, METH_VARARGS, "" },
	{ "set_material_texture_parameter", (PyCFunction)py_ue_set_material_texture_parameter, METH_VARARGS, "" },
	{ "get_material_scalar_parameter", (PyCFunction)py_ue_get_material_scalar_parameter, METH_VARARGS, "" },
	{ "get_material_vector_parameter", (PyCFunction)py_ue_get_material_vector_parameter, METH_VARARGS, "" },
	{ "get_material_texture_parameter", (PyCFunction)py_ue_get_material_texture_parameter, METH_VARARGS, "" },
	{ "create_material_instance_dynamic", (PyCFunction)py_ue_create_material_instance_dynamic, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "set_material_parent", (PyCFunction)py_ue_set_material_parent, METH_VARARGS, "" },
	{ "static_mesh_set_collision_for_lod", (PyCFunction)py_ue_static_mesh_set_collision_for_lod, METH_VARARGS, "" },
	{ "static_mesh_set_shadow_for_lod", (PyCFunction)py_ue_static_mesh_set_shadow_for_lod, METH_VARARGS, "" },
#endif



#if PY_MAJOR_VERSION >= 3
	{ "add_function", (PyCFunction)py_ue_add_function, METH_VARARGS, "" },
#endif
	{ "add_property", (PyCFunction)py_ue_add_property, METH_VARARGS, "" },
	{ "as_dict", (PyCFunction)py_ue_as_dict, METH_VARARGS, "" },

	{ "get_cdo", (PyCFunction)py_ue_get_cdo, METH_VARARGS, "" },
	{ "enum_values", (PyCFunction)py_ue_enum_values, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

void ue_pydelegates_cleanup(ue_PyUObject *self) {
	// this could happen during engine shutdown, so have mercy
	// start deallocating delegates mapped to the object
	if (!self || !self->python_delegates_gc)
		return;
	for (UPythonDelegate *py_delegate : *(self->python_delegates_gc)) {
		if (py_delegate && py_delegate->IsValidLowLevel()) {
#if UEPY_MEMORY_DEBUG
			UE_LOG(LogPython, Warning, TEXT("Removing UPythonDelegate %p from ue_PyUObject %p mapped to UObject %p"), py_delegate, self, self->ue_object);
#endif
			py_delegate->RemoveFromRoot();
}
}
	self->python_delegates_gc->clear();
	delete self->python_delegates_gc;
	self->python_delegates_gc = nullptr;

	Py_XDECREF(self->py_dict);
}

// destructor
static void ue_pyobject_dealloc(ue_PyUObject *self) {
#if UEPY_MEMORY_DEBUG
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyUObject %p mapped to UObject %p"), self, self->ue_object);
#endif
	ue_pydelegates_cleanup(self);
	ue_python_gc.erase(self->ue_object);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *ue_PyUObject_getattro(ue_PyUObject *self, PyObject *attr_name) {
	PyObject *ret = PyObject_GenericGetAttr((PyObject *)self, attr_name);
	if (!ret) {
		if (PyUnicodeOrString_Check(attr_name)) {
			char *attr = PyUnicode_AsUTF8(attr_name);
			// first check for property
			UStruct *u_struct = nullptr;
			if (self->ue_object->IsA<UStruct>()) {
				u_struct = (UStruct *)self->ue_object;
			}
			else {
				u_struct = (UStruct *)self->ue_object->GetClass();
			}
			UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(attr)));
			if (u_property) {
				// swallow previous exception
				PyErr_Clear();
				return ue_py_convert_property(u_property, (uint8 *)self->ue_object);
			}

			UFunction *function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(attr)));
			// retry wth K2_ prefix
			if (!function) {
				FString k2_name = FString("K2_") + UTF8_TO_TCHAR(attr);
				function = self->ue_object->FindFunction(FName(*k2_name));
			}

			// is it a static class ?
			if (!function) {
				if (self->ue_object->IsA<UClass>()) {
					UClass *u_class = (UClass *)self->ue_object;
					UObject *cdo = u_class->GetDefaultObject();
					if (cdo) {
						function = cdo->FindFunction(FName(UTF8_TO_TCHAR(attr)));
						// try _NEW ?
						if (!function) {
							FString name_new = UTF8_TO_TCHAR(attr) + FString("_NEW");
							function = cdo->FindFunction(FName(*name_new));
						}
					}
				}
			}

			// last hope, is it an enum ?
			if (!function) {
				if (self->ue_object->IsA<UEnum>()) {
					UEnum *u_enum = (UEnum *)self->ue_object;
					PyErr_Clear();
					return PyLong_FromLong(u_enum->FindEnumIndex(FName(UTF8_TO_TCHAR(attr))));
				}
			}

			if (function) {
				// swallow previous exception
				PyErr_Clear();
				return py_ue_new_callable(function, self->ue_object);
			}
		}
	}
	return ret;
}

static int ue_PyUObject_setattro(ue_PyUObject *self, PyObject *attr_name, PyObject *value) {
	// first of all check for UProperty
	if (PyUnicodeOrString_Check(attr_name)) {
		char *attr = PyUnicode_AsUTF8(attr_name);
		// first check for property
		UStruct *u_struct = nullptr;
		if (self->ue_object->IsA<UStruct>()) {
			u_struct = (UStruct *)self->ue_object;
		}
		else {
			u_struct = (UStruct *)self->ue_object->GetClass();
		}
		UProperty *u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(attr)));
		if (u_property) {
			if (ue_py_convert_pyobject(value, u_property, (uint8*)self->ue_object)) {
				return 0;
			}
			PyErr_SetString(PyExc_ValueError, "invalid value for UProperty");
			return -1;
		}

		// now check for function name
		if (self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(attr)))) {
			PyErr_SetString(PyExc_ValueError, "you cannot overwrite a UFunction");
			return -1;
		}
	}
	return PyObject_GenericSetAttr((PyObject *)self, attr_name, value);
}

static PyObject *ue_PyUObject_str(ue_PyUObject *self)
{
	return PyUnicode_FromFormat("<unreal_engine.UObject '%s' (%p) UClass '%s'>",
		TCHAR_TO_UTF8(*self->ue_object->GetName()), self->ue_object, TCHAR_TO_UTF8(*self->ue_object->GetClass()->GetName()));
}

static PyObject *ue_PyUObject_call(ue_PyUObject *self, PyObject *args, PyObject *kw) {
	// if it is a class, create a new object
	if (self->ue_object->IsA<UClass>()) {
		UClass *u_class = (UClass *)self->ue_object;
		if (u_class->HasAnyClassFlags(CLASS_Abstract)) {
			return PyErr_Format(PyExc_Exception, "abstract classes cannot be instantiated");
		}
		if (u_class->IsChildOf<AActor>()) {
			return PyErr_Format(PyExc_Exception, "you cannot use __call__ on actors, they have to be spawned");
		}
		PyObject *py_name = nullptr;
		PyObject *py_outer = Py_None;
		if (!PyArg_ParseTuple(args, "|OO:new_object", &py_name, &py_outer)) {
			return NULL;
		}
		int num_args = py_name ? 3 : 1;
		PyObject *py_args = PyTuple_New(num_args);
		Py_INCREF((PyObject *)self);
		PyTuple_SetItem(py_args, 0, (PyObject *)self);
		if (py_name) {
			PyTuple_SetItem(py_args, 1, py_outer);
			PyTuple_SetItem(py_args, 2, py_name);
		}
		PyObject *ret = py_unreal_engine_new_object(nullptr, py_args);
		Py_DECREF(py_args);
		return (PyObject *)ret;
	}

	return PyErr_Format(PyExc_Exception, "the specified uobject has no __call__ support");
}

static PyTypeObject ue_PyUObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UObject",             /* tp_name */
	sizeof(ue_PyUObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_pyobject_dealloc,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	(ternaryfunc)ue_PyUObject_call,                         /* tp_call */
	(reprfunc)ue_PyUObject_str,                         /* tp_str */
	(getattrofunc)ue_PyUObject_getattro, /* tp_getattro */
	(setattrofunc)ue_PyUObject_setattro, /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine generic UObject",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyUObject_methods,             /* tp_methods */
};

UClass *unreal_engine_new_uclass(char *name, UClass *outer_parent) {
	bool is_overwriting = false;

	UObject *outer = GetTransientPackage();
	UClass *parent = UObject::StaticClass();

	if (outer_parent) {
		parent = outer_parent;
		outer = parent->GetOuter();
	}

	UClass *new_object = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name));
	if (!new_object) {
		new_object = NewObject<UPythonClass>(outer, UTF8_TO_TCHAR(name), RF_Public | RF_Transient | RF_MarkAsNative);
		if (!new_object)
			return nullptr;
	}
	else {
		is_overwriting = true;
	}

	if (is_overwriting && new_object->Children) {
		UField *u_field = new_object->Children;
		while (u_field) {
			if (u_field->IsA<UFunction>()) {
				new_object->RemoveFunctionFromFunctionMap((UFunction *)u_field);
			}
			u_field = u_field->Next;
		}
		new_object->ClearFunctionMapsCaches();
		new_object->PurgeClass(true);
		new_object->Children = nullptr;
		new_object->ClassAddReferencedObjects = parent->ClassAddReferencedObjects;
	}

	new_object->PropertiesSize = 0;

	new_object->ClassConstructor = parent->ClassConstructor;
	new_object->SetSuperStruct(parent);

	new_object->PropertyLink = parent->PropertyLink;
	new_object->ClassWithin = parent->ClassWithin;
	new_object->ClassConfigName = parent->ClassConfigName;

	new_object->ClassFlags |= (parent->ClassFlags & (CLASS_Inherit | CLASS_ScriptInherit));
	new_object->ClassFlags |= CLASS_Native;

	new_object->ClassCastFlags = parent->ClassCastFlags;

	if (!is_overwriting)
		new_object->Bind();
	new_object->StaticLink(true);

	// it could be a class update
	if (is_overwriting && new_object->ClassDefaultObject) {
		new_object->GetDefaultObject()->RemoveFromRoot();
		new_object->GetDefaultObject()->ConditionalBeginDestroy();
		new_object->ClassDefaultObject = nullptr;
	}

	new_object->GetDefaultObject()->PostInitProperties();

	new_object->AssembleReferenceTokenStream();

	return new_object;
}

// hack for avoiding loops in class constructors (thanks to the Unreal.js project for the idea)
UClass *ue_py_class_constructor_placeholder = nullptr;
static void UEPyClassConstructor(UClass *u_class, const FObjectInitializer &ObjectInitializer) {
	if (UPythonClass *u_py_class_casted = Cast<UPythonClass>(u_class)) {
		ue_py_class_constructor_placeholder = u_class;
	}
	u_class->ClassConstructor(ObjectInitializer);
	ue_py_class_constructor_placeholder = nullptr;
}

static int unreal_engine_py_init(ue_PyUObject *self, PyObject *args, PyObject *kwds) {
	// is it subclassing ?
	if (PyTuple_Size(args) == 3) {
		// TODO make it smarter on error checking
		UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(PyTuple_GetItem(args, 0)))));
		UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(PyTuple_GetItem(args, 1)))));
		UE_LOG(LogPython, Warning, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(PyTuple_GetItem(args, 2)))));

		PyObject *parents = PyTuple_GetItem(args, 1);
		ue_PyUObject *parent = (ue_PyUObject *)PyTuple_GetItem(parents, 0);

		PyObject *class_attributes = PyTuple_GetItem(args, 2);

		PyObject *class_name = PyDict_GetItemString(class_attributes, (char *)"__qualname__");
		char *name = PyUnicode_AsUTF8(class_name);
		// check if parent is a uclass
		UClass *new_class = unreal_engine_new_uclass(name, (UClass *)parent->ue_object);
		if (!new_class)
			return -1;

		// map the class to the python object
		self->ue_object = new_class;

		PyObject *class_attributes_keys = PyObject_GetIter(class_attributes);
		for (;;) {
			PyObject *key = PyIter_Next(class_attributes_keys);
			if (!key) {
				if (PyErr_Occurred())
					return -1;
				break;
			}
			if (!PyUnicodeOrString_Check(key))
				continue;
			char *class_key = PyUnicode_AsUTF8(key);

			PyObject *value = PyDict_GetItem(class_attributes, key);

			if (strlen(class_key) > 2 && class_key[0] == '_' && class_key[1] == '_') {
				continue;
			}

			bool prop_added = false;

			// add simple property
			if (ue_is_pyuobject(value)) {
				ue_PyUObject *py_obj = (ue_PyUObject *)value;
				if (py_obj->ue_object->IsA<UClass>()) {
					UClass *p_class = (UClass *)py_obj->ue_object;
					if (p_class->IsChildOf<UProperty>()) {
						if (!py_ue_add_property(self, Py_BuildValue("(Os)", value, class_key))) {
							unreal_engine_py_log_error();
							return -1;
						}
						prop_added = true;
					}
					else {
						if (!py_ue_add_property(self, Py_BuildValue("(OsO)", (PyObject *)ue_get_python_wrapper(UObjectProperty::StaticClass()), class_key, value))) {
							unreal_engine_py_log_error();
							return -1;
						}
						prop_added = true;
					}
				}
			}
			// add array property
			else if (PyList_Check(value)) {
				if (PyList_Size(value) == 1) {
					PyObject *first_item = PyList_GetItem(value, 0);
					if (ue_is_pyuobject(first_item)) {
						ue_PyUObject *py_obj = (ue_PyUObject *)value;
						if (py_obj->ue_object->IsA<UClass>()) {
							UClass *p_class = (UClass *)py_obj->ue_object;
							if (p_class->IsChildOf<UProperty>()) {
								if (!py_ue_add_property(self, Py_BuildValue("(Os)", value, class_key))) {
									unreal_engine_py_log_error();
									return -1;
								}
								prop_added = true;
							}
						}
					}
				}
			}
			// function ?
			else if (PyCallable_Check(value)) {
				uint32 func_flags = FUNC_Native | FUNC_BlueprintCallable | FUNC_Public;
				PyObject *is_event = PyObject_GetAttrString(value, (char *)"event");
				if (is_event && PyObject_IsTrue(is_event)) {
					func_flags |= FUNC_Event | FUNC_BlueprintEvent;
				}
				else if (!is_event)
					PyErr_Clear();

				PyObject *is_multicast = PyObject_GetAttrString(value, (char *)"multicast");
				if (is_multicast && PyObject_IsTrue(is_multicast)) {
					func_flags |= FUNC_NetMulticast;
				}
				else if (!is_multicast)
					PyErr_Clear();
				PyObject *is_server = PyObject_GetAttrString(value, (char *)"server");
				if (is_server && PyObject_IsTrue(is_server)) {
					func_flags |= FUNC_NetServer;
				}
				else if (!is_server)
					PyErr_Clear();
				PyObject *is_client = PyObject_GetAttrString(value, (char *)"client");
				if (is_client && PyObject_IsTrue(is_client)) {
					func_flags |= FUNC_NetClient;
				}
				else if (!is_client)
					PyErr_Clear();
				PyObject *is_reliable = PyObject_GetAttrString(value, (char *)"reliable");
				if (is_reliable && PyObject_IsTrue(is_reliable)) {
					func_flags |= FUNC_NetReliable;
				}
				else if (!is_reliable)
					PyErr_Clear();


				PyObject *is_pure = PyObject_GetAttrString(value, (char *)"pure");
				if (is_pure && PyObject_IsTrue(is_pure)) {
					func_flags |= FUNC_BlueprintPure;
				}
				else if (!is_pure)
					PyErr_Clear();
				PyObject *is_static = PyObject_GetAttrString(value, (char *)"static");
				if (is_static && PyObject_IsTrue(is_static)) {
					func_flags |= FUNC_Static;
				}
				else if (!is_static)
					PyErr_Clear();
				PyObject *override_name = PyObject_GetAttrString(value, (char *)"override");
				if (override_name && PyUnicodeOrString_Check(override_name)) {
					class_key = PyUnicode_AsUTF8(override_name);
				}
				else if (override_name && PyUnicodeOrString_Check(override_name)) {
					class_key = PyUnicode_AsUTF8(override_name);
				}
				else if (!override_name)
					PyErr_Clear();
				if (!unreal_engine_add_function(new_class, class_key, value, func_flags)) {
					UE_LOG(LogPython, Error, TEXT("unable to add function %s"), UTF8_TO_TCHAR(class_key));
					return -1;
				}
				prop_added = true;
			}

			if (!prop_added) {
				PyObject_SetAttr((PyObject *)self, key, value);
			}
		}

		UPythonClass *new_u_py_class = (UPythonClass *)new_class;
		// TODO: check if we can use this to decref the ue_PyUbject mapped to the class
		new_u_py_class->py_uobject = self;
		new_u_py_class->ClassConstructor = [](const FObjectInitializer &ObjectInitializer) {
			FScopePythonGIL gil;
			UClass *u_class = ue_py_class_constructor_placeholder ? ue_py_class_constructor_placeholder : ObjectInitializer.GetClass();
			ue_py_class_constructor_placeholder = nullptr;

			UEPyClassConstructor(u_class->GetSuperClass(), ObjectInitializer);

			if (UPythonClass *u_py_class_casted = Cast<UPythonClass>(u_class)) {
				ue_PyUObject *new_self = ue_get_python_wrapper(ObjectInitializer.GetObj());
				if (!new_self) {
					unreal_engine_py_log_error();
					return;
				}

				// fill __dict__ from class
				if (u_py_class_casted->py_uobject && u_py_class_casted->py_uobject->py_dict) {
					PyObject *keys = PyDict_Keys(u_py_class_casted->py_uobject->py_dict);
					Py_ssize_t keys_len = PyList_Size(keys);
					for (Py_ssize_t i = 0; i < keys_len; i++) {
						PyObject *key = PyList_GetItem(keys, i);
						PyObject *value = PyDict_GetItem(u_py_class_casted->py_uobject->py_dict, key);
						PyObject_SetAttr((PyObject *)new_self, key, value);
					}
					Py_DECREF(keys);
				}
				// call __init__
				u_py_class_casted->CallPyConstructor(new_self);
			}
		};

		// add custom constructor (__init__)
		PyObject *py_init = PyDict_GetItemString(class_attributes, (char *)"__init__");
		if (py_init && PyCallable_Check(py_init)) {
			new_u_py_class->SetPyConstructor(py_init);
		}
	}

	return 0;
}

void unreal_engine_init_py_module() {
#if PY_MAJOR_VERSION >= 3
	PyImport_AppendInittab("unreal_engine", init_unreal_engine);
	PyObject *new_unreal_engine_module = PyImport_AddModule("unreal_engine");
#else
	PyObject *new_unreal_engine_module = Py_InitModule3("unreal_engine", NULL, unreal_engine_py_doc);
#endif


	PyObject *unreal_engine_dict = PyModule_GetDict(new_unreal_engine_module);

	PyMethodDef *unreal_engine_function;
	for (unreal_engine_function = unreal_engine_methods; unreal_engine_function->ml_name != NULL; unreal_engine_function++) {
		PyObject *func = PyCFunction_New(unreal_engine_function, NULL);
		PyDict_SetItemString(unreal_engine_dict, unreal_engine_function->ml_name, func);
		Py_DECREF(func);
	}


	ue_PyUObjectType.tp_new = PyType_GenericNew;
	ue_PyUObjectType.tp_init = (initproc)unreal_engine_py_init;
	ue_PyUObjectType.tp_dictoffset = offsetof(ue_PyUObject, py_dict);
	if (PyType_Ready(&ue_PyUObjectType) < 0)
		return;

	Py_INCREF(&ue_PyUObjectType);
	PyModule_AddObject(new_unreal_engine_module, "UObject", (PyObject *)&ue_PyUObjectType);

	ue_python_init_fvector(new_unreal_engine_module);
	ue_python_init_frotator(new_unreal_engine_module);
	ue_python_init_ftransform(new_unreal_engine_module);
	ue_python_init_fhitresult(new_unreal_engine_module);
	ue_python_init_fcolor(new_unreal_engine_module);
	ue_python_init_flinearcolor(new_unreal_engine_module);

	ue_python_init_ftimerhandle(new_unreal_engine_module);

	ue_python_init_fdelegatehandle(new_unreal_engine_module);

	ue_python_init_fsocket(new_unreal_engine_module);

	ue_python_init_callable(new_unreal_engine_module);

	ue_python_init_uclassesimporter(new_unreal_engine_module);

#if WITH_EDITOR
	ue_python_init_swidget(new_unreal_engine_module);
	ue_python_init_farfilter(new_unreal_engine_module);
	ue_python_init_fassetdata(new_unreal_engine_module);
	ue_python_init_edgraphpin(new_unreal_engine_module);
#endif

	PyObject *py_sys = PyImport_ImportModule("sys");
	PyObject *py_sys_dict = PyModule_GetDict(py_sys);

	PyObject *py_sys_modules = PyDict_GetItemString(py_sys_dict, "modules");
	PyObject *u_classes_importer = py_ue_new_uclassesimporter();
	Py_INCREF(u_classes_importer);
	PyDict_SetItemString(py_sys_modules, "unreal_engine.classes", u_classes_importer);

	PyObject *u_enums_importer = py_ue_new_enumsimporter();
	Py_INCREF(u_enums_importer);
	PyDict_SetItemString(py_sys_modules, "unreal_engine.enums", u_enums_importer);

	// Collision channels
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_CAMERA", PyLong_FromLong(ECollisionChannel::ECC_Camera));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_DESTRUCTIBLE", PyLong_FromLong(ECollisionChannel::ECC_Destructible));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_PAWN", PyLong_FromLong(ECollisionChannel::ECC_Pawn));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_PHYSICS_BODY", PyLong_FromLong(ECollisionChannel::ECC_PhysicsBody));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_VEHICLE", PyLong_FromLong(ECollisionChannel::ECC_Vehicle));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_VISIBILITY", PyLong_FromLong(ECollisionChannel::ECC_Visibility));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_WORLD_DYNAMIC", PyLong_FromLong(ECollisionChannel::ECC_WorldDynamic));
	PyDict_SetItemString(unreal_engine_dict, "COLLISION_CHANNEL_WORLD_STATIC", PyLong_FromLong(ECollisionChannel::ECC_WorldStatic));

	// Attachments
	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_KEEP_RELATIVE", PyLong_FromLong((int)EAttachmentRule::KeepRelative));
	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_KEEP_WORLD", PyLong_FromLong((int)EAttachmentRule::KeepWorld));
	PyDict_SetItemString(unreal_engine_dict, "ATTACHMENT_RULE_SNAP_TO_TARGET", PyLong_FromLong((int)EAttachmentRule::SnapToTarget));

	// Input
	PyDict_SetItemString(unreal_engine_dict, "IE_AXIS", PyLong_FromLong(EInputEvent::IE_Axis));
	PyDict_SetItemString(unreal_engine_dict, "IE_DOUBLE_CLICK", PyLong_FromLong(EInputEvent::IE_DoubleClick));
	PyDict_SetItemString(unreal_engine_dict, "IE_PRESSED", PyLong_FromLong(EInputEvent::IE_Pressed));
	PyDict_SetItemString(unreal_engine_dict, "IE_RELEASED", PyLong_FromLong(EInputEvent::IE_Released));
	PyDict_SetItemString(unreal_engine_dict, "IE_REPEAT", PyLong_FromLong(EInputEvent::IE_Repeat));

#if WITH_EDITOR
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_OK", PyLong_FromLong(EAppMsgType::Ok));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_YES_NO", PyLong_FromLong(EAppMsgType::YesNo));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_OK_CANCEL", PyLong_FromLong(EAppMsgType::OkCancel));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_YES_NO_CANCEL", PyLong_FromLong(EAppMsgType::YesNoCancel));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_CANCEL_RETRY_CONTINUE", PyLong_FromLong(EAppMsgType::CancelRetryContinue));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_YES_NO_YES_ALL_NO_ALL", PyLong_FromLong(EAppMsgType::YesNoYesAllNoAll));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_YES_NO_YES_ALL_NO_ALL_CANCEL", PyLong_FromLong(EAppMsgType::YesNoYesAllNoAllCancel));
	PyDict_SetItemString(unreal_engine_dict, "APP_MSG_TYPE_YES_NO_YES_ALL", PyLong_FromLong(EAppMsgType::YesNoYesAll));

	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_OK", PyLong_FromLong(EAppReturnType::Ok));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_YES", PyLong_FromLong(EAppReturnType::Yes));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_YES_ALL", PyLong_FromLong(EAppReturnType::YesAll));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_NO_ALL", PyLong_FromLong(EAppReturnType::NoAll));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_NO", PyLong_FromLong(EAppReturnType::No));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_RETRY", PyLong_FromLong(EAppReturnType::Retry));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_CONTINUE", PyLong_FromLong(EAppReturnType::Continue));
	PyDict_SetItemString(unreal_engine_dict, "APP_RETURN_TYPE_CANCEL", PyLong_FromLong(EAppReturnType::Cancel));

#endif
}


// utility functions

ue_PyUObject *ue_get_python_wrapper(UObject *ue_obj) {
	if (!ue_obj || !ue_obj->IsValidLowLevel() || ue_obj->IsPendingKillOrUnreachable())
		return nullptr;
	std::map<UObject *, ue_PyUObject *>::iterator it = ue_python_gc.find(ue_obj);
	// not found ??
	if (it == ue_python_gc.end()) {

		ue_PyUObject *ue_py_object = (ue_PyUObject *)PyObject_New(ue_PyUObject, &ue_PyUObjectType);
		if (!ue_py_object) {
			return nullptr;
		}
		ue_py_object->ue_object = ue_obj;
		ue_py_object->python_delegates_gc = new std::list<UPythonDelegate *>;
		ue_py_object->py_dict = PyDict_New();

		ue_python_gc[ue_obj] = ue_py_object;

#if UEPY_MEMORY_DEBUG
		UE_LOG(LogPython, Warning, TEXT("CREATED UPyObject at %p for %p %s"), ue_py_object, ue_obj, *ue_obj->GetName());
#endif
		//Py_INCREF(ue_py_object);
		return ue_py_object;
	}

	return it->second;
}

void unreal_engine_py_log_error() {
	PyObject *type = NULL;
	PyObject *value = NULL;
	PyObject *traceback = NULL;

	PyErr_Fetch(&type, &value, &traceback);
	PyErr_NormalizeException(&type, &value, &traceback);

	if (!value) {
		PyErr_Clear();
		return;
	}

	char *msg = NULL;
#if PY_MAJOR_VERSION >= 3
	PyObject *zero = PyUnicode_AsUTF8String(PyObject_Str(value));
	if (zero) {
		msg = PyBytes_AsString(zero);
	}
#else
	msg = PyString_AsString(PyObject_Str(value));
#endif
	if (!msg) {
		PyErr_Clear();
		return;
	}

	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));

	// taken from uWSGI ;)
	if (!traceback) {
		PyErr_Clear();
		return;
	}

	PyObject *traceback_module = PyImport_ImportModule("traceback");
	if (!traceback_module) {
		PyErr_Clear();
		return;
	}

	PyObject *traceback_dict = PyModule_GetDict(traceback_module);
	PyObject *format_exception = PyDict_GetItemString(traceback_dict, "format_exception");

	if (format_exception) {
		PyObject *ret = PyObject_CallFunctionObjArgs(format_exception, type, value, traceback, NULL);
		if (!ret) {
			PyErr_Clear();
			return;
		}
		if (PyList_Check(ret)) {
			for (int i = 0; i < PyList_Size(ret); i++) {
				PyObject *item = PyList_GetItem(ret, i);
				if (item) {
					UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(item))));
				}
			}
		}
		else {
			UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(PyUnicode_AsUTF8(PyObject_Str(ret))));
		}
	}

	PyErr_Clear();
	}

// retrieve a UWorld from a generic UObject (if possible)
UWorld *ue_get_uworld(ue_PyUObject *py_obj) {

	if (py_obj->ue_object->IsA<UWorld>()) {
		return (UWorld *)py_obj->ue_object;
	}

	if (py_obj->ue_object->IsA<AActor>()) {
		AActor *actor = (AActor *)py_obj->ue_object;
		return actor->GetWorld();
	}

	if (py_obj->ue_object->IsA<UActorComponent>()) {
		UActorComponent *component = (UActorComponent *)py_obj->ue_object;
		return component->GetWorld();
	}

	return nullptr;
}

// retrieve actor from component (if possible)
AActor *ue_get_actor(ue_PyUObject *py_obj) {
	if (py_obj->ue_object->IsA<AActor>()) {
		return (AActor *)py_obj->ue_object;
	}

	if (py_obj->ue_object->IsA<UActorComponent>()) {
		UActorComponent *tmp_component = (UActorComponent *)py_obj->ue_object;
		return tmp_component->GetOwner();
	}
	return nullptr;
}

// convert a property to a python object
PyObject *ue_py_convert_property(UProperty *prop, uint8 *buffer) {
	if (auto casted_prop = Cast<UBoolProperty>(prop)) {
		bool value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			Py_INCREF(Py_True);
			return Py_True;
		}
		Py_INCREF(Py_False);
		return Py_False;
	}

	if (auto casted_prop = Cast<UIntProperty>(prop)) {
		int value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyLong_FromLong(value);
	}

	if (auto casted_prop = Cast<UInt64Property>(prop)) {
		long long value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyLong_FromLongLong(value);
	}

	if (auto casted_prop = Cast<UFloatProperty>(prop)) {
		float value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyFloat_FromDouble(value);
	}

	if (auto casted_prop = Cast<UByteProperty>(prop)) {
		int8 value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyLong_FromLong(value);
	}

	if (auto casted_prop = Cast<UStrProperty>(prop)) {
		FString value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	if (auto casted_prop = Cast<UTextProperty>(prop)) {
		FText value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value.ToString()));
	}

	if (auto casted_prop = Cast<UNameProperty>(prop)) {
		FName value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value.ToString()));
	}

	if (auto casted_prop = Cast<UObjectPropertyBase>(prop)) {
		auto value = casted_prop->GetObjectPropertyValue_InContainer(buffer);
		if (value) {
			ue_PyUObject *ret = ue_get_python_wrapper(value);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (auto casted_prop = Cast<UClassProperty>(prop)) {
		auto value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			ue_PyUObject *ret = ue_get_python_wrapper(value);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		return PyErr_Format(PyExc_Exception, "invalid UClass type for %s", TCHAR_TO_UTF8(*casted_prop->GetName()));
	}

	// map a UStruct to a dictionary (if possible)
	if (auto casted_prop = Cast<UStructProperty>(prop)) {
		if (auto casted_struct = Cast<UScriptStruct>(casted_prop->Struct)) {
			// check for FVector
			if (casted_struct == TBaseStructure<FVector>::Get()) {
				FVector vec = *casted_prop->ContainerPtrToValuePtr<FVector>(buffer);
				return py_ue_new_fvector(vec);
			}
			if (casted_struct == TBaseStructure<FRotator>::Get()) {
				FRotator rot = *casted_prop->ContainerPtrToValuePtr<FRotator>(buffer);
				return py_ue_new_frotator(rot);
			}
			if (casted_struct == TBaseStructure<FTransform>::Get()) {
				FTransform transform = *casted_prop->ContainerPtrToValuePtr<FTransform>(buffer);
				return py_ue_new_ftransform(transform);
			}
			if (casted_struct == FHitResult::StaticStruct()) {
				FHitResult hit = *casted_prop->ContainerPtrToValuePtr<FHitResult>(buffer);
				return py_ue_new_fhitresult(hit);
			}
			if (casted_struct == TBaseStructure<FColor>::Get()) {
				FColor color = *casted_prop->ContainerPtrToValuePtr<FColor>(buffer);
				return py_ue_new_fcolor(color);
			}
			if (casted_struct == TBaseStructure<FLinearColor>::Get()) {
				FLinearColor color = *casted_prop->ContainerPtrToValuePtr<FLinearColor>(buffer);
				return py_ue_new_flinearcolor(color);
			}
			ue_PyUObject *ret = ue_get_python_wrapper(casted_struct);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		return PyErr_Format(PyExc_TypeError, "unsupported UStruct type");
	}

	if (auto casted_prop = Cast<UWeakObjectProperty>(prop)) {
		auto value = casted_prop->GetPropertyValue_InContainer(buffer);
		UObject *strong_obj = value.Get();
		if (strong_obj) {
			ue_PyUObject *ret = ue_get_python_wrapper(strong_obj);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		// nullptr
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(prop)) {
		ue_PyUObject *ret = ue_get_python_wrapper(casted_prop);
		if (!ret)
			return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
		Py_INCREF(ret);
		return (PyObject *)ret;
	}

	if (auto casted_prop = Cast<UArrayProperty>(prop)) {
		FScriptArrayHelper_InContainer array_helper(casted_prop, buffer);
		PyObject *py_list = PyList_New(0);
		UProperty *array_prop = casted_prop->Inner;
		if (array_prop->IsA<UStructProperty>()) {
			uint8 *array_buffer = (uint8 *)FMemory_Alloca(array_prop->GetSize());
			for (int i = 0; i < array_helper.Num(); i++) {
				array_prop->InitializeValue(array_buffer);
				array_prop->CopyCompleteValueFromScriptVM(array_buffer, array_helper.GetRawPtr(i));
				PyObject *item = ue_py_convert_property(array_prop, array_buffer);
				array_prop->DestroyValue(array_buffer);
				if (!item) {
					Py_DECREF(py_list);
					return NULL;
				}
				PyList_Append(py_list, item);
				Py_DECREF(item);
			}
		}
		else {
			for (int i = 0; i < array_helper.Num(); i++) {
				PyObject *item = ue_py_convert_property(array_prop, array_helper.GetRawPtr(i));
				if (!item) {
					Py_DECREF(py_list);
					return NULL;
				}
				PyList_Append(py_list, item);
				Py_DECREF(item);
			}
		}
		return py_list;
	}

	return PyErr_Format(PyExc_Exception, "unsupported value type %s for property %s", TCHAR_TO_UTF8(*prop->GetClass()->GetName()), TCHAR_TO_UTF8(*prop->GetName()));
}

// convert a python object to a property
bool ue_py_convert_pyobject(PyObject *py_obj, UProperty *prop, uint8 *buffer) {

	if (PyBool_Check(py_obj)) {
		auto casted_prop = Cast<UBoolProperty>(prop);
		if (!casted_prop)
			return false;
		if (PyObject_IsTrue(py_obj)) {
			casted_prop->SetPropertyValue_InContainer(buffer, true);
		}
		else {
			casted_prop->SetPropertyValue_InContainer(buffer, false);
		}
		return true;
	}

	if (PyNumber_Check(py_obj)) {
		if (auto casted_prop = Cast<UIntProperty>(prop)) {
			PyObject *py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLong(py_long));
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UInt64Property>(prop)) {
			PyObject *py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLongLong(py_long));
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UFloatProperty>(prop)) {
			PyObject *py_float = PyNumber_Float(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyFloat_AsDouble(py_float));
			Py_DECREF(py_float);
			return true;
		}
		if (auto casted_prop = Cast<UByteProperty>(prop)) {
			PyObject *py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLong(py_long));
			Py_DECREF(py_long);
			return true;
		}
		return false;
	}

	if (PyUnicodeOrString_Check(py_obj)) {
		if (auto casted_prop = Cast<UStrProperty>(prop)) {
			casted_prop->SetPropertyValue_InContainer(buffer, UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj)));
			return true;
		}
		if (auto casted_prop = Cast<UNameProperty>(prop)) {
			casted_prop->SetPropertyValue_InContainer(buffer, UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj)));
			return true;
		}
		if (auto casted_prop = Cast<UTextProperty>(prop)) {
			casted_prop->SetPropertyValue_InContainer(buffer, FText::FromString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj))));
			return true;
		}
		return false;
	}

	if (PyList_Check(py_obj)) {
		if (auto casted_prop = Cast<UArrayProperty>(prop)) {
			FScriptArrayHelper_InContainer helper(casted_prop, buffer);

			UProperty *array_prop = casted_prop->Inner;
			Py_ssize_t pylist_len = PyList_Size(py_obj);

			// fix array helper size
			if (helper.Num() < pylist_len) {
				helper.AddValues(pylist_len - helper.Num());
			}
			else if (helper.Num() > pylist_len) {
				helper.RemoveValues(pylist_len, helper.Num() - pylist_len);
			}

			for (int i = 0; i < (int)pylist_len; i++) {
				uint8 *item_buf = helper.GetRawPtr(i);
				PyObject *py_item = PyList_GetItem(py_obj, i);
				if (!ue_py_convert_pyobject(py_item, array_prop, item_buf)) {
					Py_DECREF(py_item);
					return false;
				}
				Py_DECREF(py_item);
			}
			return true;
		}
		return false;
	}

	// TODO encode a dictionary to a struct
	if (PyDict_Check(py_obj)) {
		return false;
	}

	if (ue_PyFVector *py_vec = py_ue_is_fvector(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == TBaseStructure<FVector>::Get()) {
				*casted_prop->ContainerPtrToValuePtr<FVector>(buffer) = py_vec->vec;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFRotator *py_rot = py_ue_is_frotator(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == TBaseStructure<FRotator>::Get()) {
				*casted_prop->ContainerPtrToValuePtr<FRotator>(buffer) = py_rot->rot;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFTransform *py_transform = py_ue_is_ftransform(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == TBaseStructure<FTransform>::Get()) {
				*casted_prop->ContainerPtrToValuePtr<FTransform>(buffer) = py_transform->transform;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFColor *py_color = py_ue_is_fcolor(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == TBaseStructure<FColor>::Get()) {
				*casted_prop->ContainerPtrToValuePtr<FColor>(buffer) = py_color->color;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == TBaseStructure<FLinearColor>::Get()) {
				*casted_prop->ContainerPtrToValuePtr<FLinearColor>(buffer) = py_color->color;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFHitResult *py_hit = py_ue_is_fhitresult(py_obj)) {
		if (auto casted_prop = Cast<UStructProperty>(prop)) {
			if (casted_prop->Struct == FHitResult::StaticStruct()) {
				*casted_prop->ContainerPtrToValuePtr<FHitResult>(buffer) = py_hit->hit;
				return true;
			}
		}
		return false;
	}

	if (PyObject_IsInstance(py_obj, (PyObject *)&ue_PyUObjectType)) {
		ue_PyUObject *ue_obj = (ue_PyUObject *)py_obj;
		if (ue_obj->ue_object->IsA<UClass>()) {
			auto casted_prop = Cast<UClassProperty>(prop);
			if (!casted_prop)
				return false;
			casted_prop->SetPropertyValue_InContainer(buffer, (UClass *)ue_obj->ue_object);
			return true;
		}

		if (ue_obj->ue_object->IsA<UObject>()) {
			auto casted_prop = Cast<UObjectPropertyBase>(prop);
			if (!casted_prop)
				return false;
			// ensure the object type is correct, otherwise crash could happen (soon or later)
			if (!ue_obj->ue_object->IsA(casted_prop->PropertyClass))
				return false;
			casted_prop->SetObjectPropertyValue_InContainer(buffer, ue_obj->ue_object);
			return true;
		}
	}

	return false;

}

// check if a python object is a wrapper to a UObject
ue_PyUObject *ue_is_pyuobject(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType))
		return nullptr;
	return (ue_PyUObject *)obj;
}

// automatically bind events based on class methods names
void ue_autobind_events_for_pyclass(ue_PyUObject *u_obj, PyObject *py_class) {
	PyObject *attrs = PyObject_Dir(py_class);
	if (!attrs)
		return;

	Py_ssize_t len = PyList_Size(attrs);
	for (Py_ssize_t i = 0; i < len; i++) {
		PyObject *py_attr_name = PyList_GetItem(attrs, i);
		if (!py_attr_name || !PyUnicodeOrString_Check(py_attr_name))
			continue;
		FString attr_name = UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_attr_name));
		if (!attr_name.StartsWith("on_", ESearchCase::CaseSensitive))
			continue;
		// check if the attr is a callable
		PyObject *item = PyObject_GetAttrString(py_class, TCHAR_TO_UTF8(*attr_name));
		if (item && PyCallable_Check(item)) {
			TArray<FString> parts;
			if (attr_name.ParseIntoArray(parts, UTF8_TO_TCHAR("_")) > 1) {
				FString event_name;
				for (FString part : parts) {
					FString first_letter = part.Left(1).ToUpper();
					part.RemoveAt(0);
					event_name = event_name.Append(first_letter);
					event_name = event_name.Append(part);
				}
				// do not fail on wrong properties
				ue_bind_pyevent(u_obj, event_name, item, false);
			}
		}
		Py_XDECREF(item);
	}

	Py_DECREF(attrs);
}

static void py_ue_destroy_params(UFunction *u_function, uint8 *buffer) {
	// destroy params
	TFieldIterator<UProperty> DArgs(u_function);
	for (; DArgs && (DArgs->PropertyFlags & CPF_Parm); ++DArgs) {
		UProperty *prop = *DArgs;
		prop->DestroyValue_InContainer(buffer);
	}
}

PyObject *py_ue_ufunction_call(UFunction *u_function, UObject *u_obj, PyObject *args, int argn, PyObject *kwargs) {

	// check for __super call
	if (kwargs) {
		PyObject *is_super_call = PyDict_GetItemString(kwargs, (char *)"__super");
		if (is_super_call) {
			if (!u_function->GetSuperFunction()) {
				return PyErr_Format(PyExc_Exception, "UFunction has no SuperFunction");
			}
			u_function = u_function->GetSuperFunction();
		}
	}

	uint8 *buffer = (uint8 *)FMemory_Alloca(u_function->ParmsSize);
	FMemory::Memzero(buffer, u_function->ParmsSize);

	// initialize args
	TFieldIterator<UProperty> IArgs(u_function);
	for (; IArgs && (IArgs->PropertyFlags & CPF_Parm); ++IArgs) {
		UProperty *prop = *IArgs;
		prop->InitializeValue_InContainer(buffer);
#if WITH_EDITOR
		FString default_key = FString("CPP_Default_") + prop->GetName();
		FString default_key_value = u_function->GetMetaData(FName(*default_key));
		if (!default_key_value.IsEmpty()) {
			prop->ImportText(*default_key_value, prop->ContainerPtrToValuePtr<uint8>(buffer), PPF_Localized, NULL);
		}
#endif
	}

	Py_ssize_t tuple_len = PyTuple_Size(args);

	int has_out_params = 0;

	TFieldIterator<UProperty> PArgs(u_function);
	for (; PArgs && ((PArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++PArgs) {
		UProperty *prop = *PArgs;
		if (argn < tuple_len) {
			PyObject *py_arg = PyTuple_GetItem(args, argn);
			if (!py_arg) {
				py_ue_destroy_params(u_function, buffer);
				return PyErr_Format(PyExc_TypeError, "unable to get pyobject for property %s", TCHAR_TO_UTF8(*prop->GetName()));
			}
			if (!ue_py_convert_pyobject(py_arg, prop, buffer)) {
				py_ue_destroy_params(u_function, buffer);
				return PyErr_Format(PyExc_TypeError, "unable to convert pyobject to property %s (%s)", TCHAR_TO_UTF8(*prop->GetName()), TCHAR_TO_UTF8(*prop->GetClass()->GetName()));
			}
		}
		else if (kwargs) {
			char *prop_name = TCHAR_TO_UTF8(*prop->GetName());
			PyObject *dict_value = PyDict_GetItemString(kwargs, prop_name);
			if (dict_value) {
				if (!ue_py_convert_pyobject(dict_value, prop, buffer)) {
					py_ue_destroy_params(u_function, buffer);
					return PyErr_Format(PyExc_TypeError, "unable to convert pyobject to property %s (%s)", TCHAR_TO_UTF8(*prop->GetName()), TCHAR_TO_UTF8(*prop->GetClass()->GetName()));
				}
			}
		}
		if ((PArgs->PropertyFlags & (CPF_ConstParm | CPF_OutParm)) == CPF_OutParm) {
			has_out_params++;
		}
		argn++;
	}

	FScopeCycleCounterUObject ObjectScope(u_obj);
	FScopeCycleCounterUObject FunctionScope(u_function);

	u_obj->ProcessEvent(u_function, buffer);

	PyObject *ret = nullptr;

	int has_ret_param = 0;
	TFieldIterator<UProperty> Props(u_function);
	for (; Props; ++Props) {
		UProperty *prop = *Props;
		if (prop->GetPropertyFlags() & CPF_ReturnParm) {
			ret = ue_py_convert_property(prop, buffer);
			if (!ret) {
				// destroy params
				py_ue_destroy_params(u_function, buffer);
				return NULL;
			}
			has_ret_param = 1;
			break;
		}
	}

	if (has_out_params > 0) {
		PyObject *multi_ret = PyTuple_New(has_out_params + has_ret_param);
		if (ret) {
			PyTuple_SetItem(multi_ret, 0, ret);
		}
		TFieldIterator<UProperty> OProps(u_function);
		for (; OProps; ++OProps) {
			UProperty *prop = *OProps;
			if ((prop->GetPropertyFlags() & (CPF_ConstParm | CPF_OutParm)) == CPF_OutParm) {
				// skip return param as it must be always the first
				if (prop->GetPropertyFlags() & CPF_ReturnParm)
					continue;
				PyObject *py_out = ue_py_convert_property(prop, buffer);
				if (!py_out) {
					Py_DECREF(multi_ret);
					// destroy params
					py_ue_destroy_params(u_function, buffer);
					return NULL;
				}
				PyTuple_SetItem(multi_ret, has_ret_param, py_out);
				has_ret_param++;
			}
		}
		// destroy params
		py_ue_destroy_params(u_function, buffer);
		return multi_ret;
	}

	// destroy params
	py_ue_destroy_params(u_function, buffer);

	if (ret)
		return ret;

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *ue_bind_pyevent(ue_PyUObject *u_obj, FString event_name, PyObject *py_callable, bool fail_on_wrong_property) {

	UProperty *u_property = u_obj->ue_object->GetClass()->FindPropertyByName(FName(*event_name));
	if (!u_property) {
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "unable to find event property %s", TCHAR_TO_UTF8(*event_name));
		Py_INCREF(Py_None);
		return Py_None;
	}

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(u_property)) {
		FMulticastScriptDelegate multiscript_delegate = casted_prop->GetPropertyValue_InContainer(u_obj->ue_object);

		FScriptDelegate script_delegate;
		UPythonDelegate *py_delegate = NewObject<UPythonDelegate>();
		py_delegate->SetPyCallable(py_callable);
		py_delegate->SetSignature(casted_prop->SignatureFunction);
		// avoid delegates to be destroyed by the GC
		py_delegate->AddToRoot();
		u_obj->python_delegates_gc->push_back(py_delegate);

		// fake UFUNCTION for bypassing checks
		script_delegate.BindUFunction(py_delegate, FName("PyFakeCallable"));

		// add the new delegate
		multiscript_delegate.Add(script_delegate);

		// re-assign multicast delegate
		casted_prop->SetPropertyValue_InContainer(u_obj->ue_object, multiscript_delegate);
	}
	else {
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "property %s is not an event", TCHAR_TO_UTF8(*event_name));
	}

	Py_INCREF(Py_None);
	return Py_None;
}

UFunction *unreal_engine_add_function(UClass *u_class, char *name, PyObject *py_callable, uint32 function_flags) {

	UFunction *parent_function = u_class->GetSuperClass()->FindFunctionByName(UTF8_TO_TCHAR(name));
	// if the function is not available in the parent
	// check for name collision
	if (!parent_function) {
		if (u_class->FindFunctionByName(UTF8_TO_TCHAR(name))) {
			UE_LOG(LogPython, Error, TEXT("function %s is already registered"), UTF8_TO_TCHAR(name));
			return nullptr;
		}
	}

	UPythonFunction *function = NewObject<UPythonFunction>(u_class, UTF8_TO_TCHAR(name), RF_Public | RF_Transient | RF_MarkAsNative);
	function->SetPyCallable(py_callable);

	function->RepOffset = MAX_uint16;
	function->ReturnValueOffset = MAX_uint16;
	function->FirstPropertyToInit = NULL;
	function->Script.Add(EX_EndFunctionParms);

	if (parent_function) {
		function->SetSuperStruct(parent_function);
		function_flags |= (parent_function->FunctionFlags & FUNC_FuncInherit);
	}

	// iterate all arguments using inspect.signature()
	// this is required to maintaining args order
	PyObject *inspect = PyImport_ImportModule("inspect");
	if (!inspect) {
		return NULL;
	}
	PyObject *signature = PyObject_CallMethod(inspect, (char *)"signature", (char *)"O", py_callable);
	if (!signature) {
		return NULL;
	}

	PyObject *parameters = PyObject_GetAttrString(signature, "parameters");
	if (!parameters) {
		return NULL;
	}

	PyObject *annotations = PyObject_GetAttrString(py_callable, "__annotations__");

	UField **next_property = &function->Children;
	UProperty **next_property_link = &function->PropertyLink;

	PyObject *parameters_keys = PyObject_GetIter(parameters);
	// do not process args if no annotations are available

	while (annotations) {
		PyObject *key = PyIter_Next(parameters_keys);
		if (!key) {
			if (PyErr_Occurred())
				return NULL;
			break;
		}
		if (!PyUnicodeOrString_Check(key))
			continue;

		char *p_name = PyUnicode_AsUTF8(key);

		PyObject *value = PyDict_GetItem(annotations, key);
		if (!value)
			continue;

		UProperty *prop = nullptr;
		if (PyType_Check(value)) {
			if ((PyTypeObject *)value == &PyFloat_Type) {
				prop = NewObject<UFloatProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject *)value == &PyUnicode_Type) {
				prop = NewObject<UStrProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject *)value == &PyBool_Type) {
				prop = NewObject<UBoolProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject *)value == &PyLong_Type) {
				prop = NewObject<UInt64Property>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
		}
		else if (ue_PyUObject *py_obj = ue_is_pyuobject(value)) {
			if (py_obj->ue_object->IsA<UClass>()) {
				UObjectProperty *prop_base = NewObject<UObjectProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_base->SetPropertyClass((UClass *)py_obj->ue_object);
				prop = prop_base;
			}
		}
		// TODO add native types (like vectors, rotators...)
		if (prop) {
			prop->SetPropertyFlags(CPF_Parm);
			*next_property = prop;
			next_property = &prop->Next;
			*next_property_link = prop;
			next_property_link = &prop->PropertyLinkNext;
		}
		else {
			UE_LOG(LogPython, Warning, TEXT("Unable to map argument %s to function %s"), UTF8_TO_TCHAR(p_name), UTF8_TO_TCHAR(name));
		}
	}

	// check for return value
	if (annotations) {
		PyObject *py_return_value = PyDict_GetItemString(annotations, "return");
		if (py_return_value) {
			UProperty *prop = nullptr;
			char *p_name = (char *) "ReturnValue";
			if (PyType_Check(py_return_value)) {
				if ((PyTypeObject *)py_return_value == &PyFloat_Type) {
					prop = NewObject<UFloatProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject *)py_return_value == &PyUnicode_Type) {
					prop = NewObject<UStrProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject *)py_return_value == &PyBool_Type) {
					prop = NewObject<UBoolProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject *)py_return_value == &PyLong_Type) {
					prop = NewObject<UInt64Property>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
			}
			else if (ue_PyUObject *py_obj = ue_is_pyuobject(py_return_value)) {
				if (py_obj->ue_object->IsA<UClass>()) {
					UObjectProperty *prop_base = NewObject<UObjectProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_base->SetPropertyClass((UClass *)py_obj->ue_object);
					prop = prop_base;
				}
			}
			// TODO add native types (like vectors, rotators...)
			if (prop) {
				prop->SetPropertyFlags(CPF_Parm | CPF_ReturnParm);
				*next_property = prop;
				next_property = &prop->Next;
				*next_property_link = prop;
				next_property_link = &prop->PropertyLinkNext;
			}
			else {
				UE_LOG(LogPython, Warning, TEXT("Unable to map return value to function %s"), UTF8_TO_TCHAR(name));
			}
		}
	}

	if (parent_function) {
		if (!parent_function->IsSignatureCompatibleWith(function)) {
			UE_LOG(LogPython, Error, TEXT("function %s signature's is not compatible with the parent"), UTF8_TO_TCHAR(name));
			return nullptr;
		}
	}

	function->Bind();
	function->StaticLink(true);

	function->ParmsSize = 0;
	function->NumParms = 0;

	// allocate properties storage (ignore super)
	TFieldIterator<UProperty> props(function, EFieldIteratorFlags::ExcludeSuper);
	for (; props; ++props) {
		UProperty *p = *props;
		if (p->HasAnyPropertyFlags(CPF_Parm)) {
			function->NumParms++;
			function->ParmsSize = p->GetOffset_ForUFunction() + p->GetSize();
			if (p->HasAnyPropertyFlags(CPF_ReturnParm)) {
				function->ReturnValueOffset = p->GetOffset_ForUFunction();
			}
		}
	}

	UE_LOG(LogPython, Warning, TEXT("REGISTERED FUNCTION %s WITH %d PARAMS (size %d) %d"), *function->GetFName().ToString(), function->NumParms, function->ParmsSize, function->PropertiesSize);

	function->FunctionFlags = function_flags;

	function->SetNativeFunc((Native)&UPythonFunction::CallPythonCallable);

	function->Next = u_class->Children;



	u_class->Children = function;
	u_class->AddFunctionToFunctionMap(function);

	u_class->StaticLink(true);

	// regenerate CDO
	u_class->GetDefaultObject()->RemoveFromRoot();
	u_class->GetDefaultObject()->ConditionalBeginDestroy();
	u_class->ClassDefaultObject = nullptr;
	u_class->GetDefaultObject()->PostInitProperties();

	return function;
}
