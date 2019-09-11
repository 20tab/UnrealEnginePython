#include "UEPyModule.h"

#include "UEPyEngine.h"
#include "UEPyTimer.h"
#include "UEPyTicker.h"
#include "UEPyVisualLogger.h"

#include "UObject/UEPyObject.h"
#include "UObject/UEPyActor.h"
#include "UObject/UEPyTransform.h"
#include "UObject/UEPyPlayer.h"
#include "UObject/UEPyInput.h"
#include "UObject/UEPyWorld.h"
#include "UObject/UEPyNavigation.h"
#include "UObject/UEPySpline.h"
#include "UObject/UEPyMovements.h"
#include "UObject/UEPyAttaching.h"
#include "UObject/UEPySkeletal.h"
#include "UObject/UEPyStaticMesh.h"
#include "UObject/UEPyTraceAndSweep.h"
#include "UObject/UEPyPhysics.h"
#include "UObject/UEPyAudio.h"
#include "UObject/UEPySequencer.h"
#include "UObject/UEPyViewport.h"
#include "UObject/UEPyWidget.h"
#include "UObject/UEPyWidgetComponent.h"
#include "UObject/UEPyPackage.h"
#include "UObject/UEPyTexture.h"
#include "UObject/UEPyMaterial.h"
#include "UObject/UEPyPawn.h"
#include "UObject/UEPyController.h"
#include "UObject/UEPyHUD.h"
#include "UObject/UEPyAnimSequence.h"
#include "UObject/UEPyCapture.h"
#include "UObject/UEPyLandscape.h"
#include "UObject/UEPyUserDefinedStruct.h"
#include "UObject/UEPyDataTable.h"
#include "UObject/UEPyExporter.h"
#include "UObject/UEPyFoliage.h"


#include "UEPyAssetUserData.h"
#if WITH_EDITOR
#include "UEPyEditor.h"
#include "Blueprint/UEPyEdGraph.h"
#include "Fbx/UEPyFbx.h"
#include "Editor/BlueprintGraph/Classes/EdGraphSchema_K2.h"
#include "Editor/BlueprintGraph/Public/BlueprintActionDatabase.h"
#endif

#include "Wrappers/UEPyESlateEnums.h"

#include "Wrappers/UEPyFVector.h"
#include "Wrappers/UEPyFVector2D.h"
#include "Wrappers/UEPyFHitResult.h"
#include "Wrappers/UEPyFRotator.h"
#include "Wrappers/UEPyFTransform.h"
#include "Wrappers/UEPyFColor.h"
#include "Wrappers/UEPyFLinearColor.h"
#include "Wrappers/UEPyFSocket.h"
#include "Wrappers/UEPyFQuat.h"

#include "Wrappers/UEPyFRawAnimSequenceTrack.h"

#include "Wrappers/UEPyFRandomStream.h"

#include "Wrappers/UEPyFPythonOutputDevice.h"
#if WITH_EDITOR
#include "Wrappers/UEPyFSoftSkinVertex.h"
#endif
#include "Wrappers/UEPyFMorphTargetDelta.h"
#include "Wrappers/UEPyFObjectThumbnail.h"

#include "Wrappers/UEPyFViewportClient.h"
#if WITH_EDITOR
#include "Wrappers/UEPyFEditorViewportClient.h"
#include "Wrappers/UEPyIAssetEditorInstance.h"
#endif

#include "Wrappers/UEPyFFoliageInstance.h"

#include "UEPyCallable.h"
#include "UEPyUClassesImporter.h"
#include "UEPyEnumsImporter.h"
#include "UEPyUStructsImporter.h"

#include "UEPyUScriptStruct.h"

#if WITH_EDITOR
#include "Wrappers/UEPyFSlowTask.h"
#include "Wrappers/UEPyFAssetData.h"
#include "Wrappers/UEPyFARFilter.h"
#include "Wrappers/UEPyFRawMesh.h"
#include "Wrappers/UEPyFStringAssetReference.h"

#include "UObject/UEPyAnimSequence.h"
#include "Blueprint/UEPyEdGraphPin.h"
#include "UEPyIPlugin.h"
#include "CollectionManager/UEPyICollectionManager.h"
#include "MaterialEditorUtilities/UEPyFMaterialEditorUtilities.h"
#endif

#include "Wrappers/UEPyFFrameNumber.h"

#include "Slate/UEPySlate.h"
#include "Http/UEPyIHttp.h"
#include "ConsoleManager/UEPyIConsoleManager.h"
#include "SlateApplication/UEPyFSlateApplication.h"
#include "Voice/UEPyIVoiceCapture.h"

#include "PythonFunction.h"
#include "PythonClass.h"

#if ENGINE_MINOR_VERSION >= 15
#include "Engine/UserDefinedEnum.h"
#endif

#include "Runtime/Core/Public/UObject/PropertyPortFlags.h"

#if ENGINE_MINOR_VERSION < 18
#define USoftObjectProperty UAssetObjectProperty
#define USoftClassProperty UAssetClassProperty
typedef FAssetPtr FSoftObjectPtr;
#endif

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
static PyObject* init_unreal_engine(void);



void init_unreal_engine_builtin()
{
	PyImport_AppendInittab("unreal_engine", &init_unreal_engine);
}
#endif


static PyObject* py_unreal_engine_py_gc(PyObject* self, PyObject* args)
{
	int32 Garbaged = FUnrealEnginePythonHouseKeeper::Get()->RunGC();
	return PyLong_FromLong(Garbaged);

}

static PyObject* py_unreal_engine_exec(PyObject* self, PyObject* args)
{
	char* filename = nullptr;
	if (!PyArg_ParseTuple(args, "s:exec", &filename))
	{
		return NULL;
	}
	FUnrealEnginePythonModule& PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	Py_BEGIN_ALLOW_THREADS;
	PythonModule.RunFile(filename);
	Py_END_ALLOW_THREADS;
	Py_RETURN_NONE;
}


#if PLATFORM_MAC
static PyObject* py_unreal_engine_exec_in_main_thread(PyObject* self, PyObject* args)
{
	char* filename = nullptr;
	if (!PyArg_ParseTuple(args, "s:exec_in_main_thread", &filename))
	{
		return NULL;
	}
	FUnrealEnginePythonModule& PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	Py_BEGIN_ALLOW_THREADS;
	PythonModule.RunFileInMainThread(filename);
	Py_END_ALLOW_THREADS;
	Py_RETURN_NONE;
}
#endif

static PyObject* py_ue_get_py_proxy(ue_PyUObject* self, PyObject* args)
{

	ue_py_check(self);

	if (self->py_proxy)
	{
		Py_INCREF(self->py_proxy);
		return (PyObject*)self->py_proxy;
	}

	Py_RETURN_NONE;
}

static PyObject* py_unreal_engine_shutdown(PyObject* self, PyObject* args)
{

	GIsRequestingExit = true;

	Py_RETURN_NONE;
}

static PyObject* py_unreal_engine_set_brutal_finalize(PyObject* self, PyObject* args)
{

	PyObject* py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|O:set_brutal_finalize", &py_bool))
	{
		return nullptr;
	}

	bool bBrutalFinalize = !py_bool || PyObject_IsTrue(py_bool);

	FUnrealEnginePythonModule& PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	PythonModule.BrutalFinalize = bBrutalFinalize;
	Py_RETURN_NONE;
}

static PyMethodDef unreal_engine_methods[] = {
	{ "log", py_unreal_engine_log, METH_VARARGS, "" },
	{ "log_warning", py_unreal_engine_log_warning, METH_VARARGS, "" },
	{ "log_error", py_unreal_engine_log_error, METH_VARARGS, "" },

	{ "shutdown", py_unreal_engine_shutdown, METH_VARARGS, "" },
	{ "set_brutal_finalize", py_unreal_engine_set_brutal_finalize, METH_VARARGS, "" },

	{ "add_on_screen_debug_message", py_unreal_engine_add_on_screen_debug_message, METH_VARARGS, "" },
	{ "print_string", py_unreal_engine_print_string, METH_VARARGS, "" },

	{ "set_random_seed", py_unreal_engine_set_random_seed, METH_VARARGS, "" },

	{ "find_class", py_unreal_engine_find_class, METH_VARARGS, "" },
	{ "find_struct", py_unreal_engine_find_struct, METH_VARARGS, "" },
	{ "find_enum", py_unreal_engine_find_enum, METH_VARARGS, "" },

	{ "load_class", py_unreal_engine_load_class, METH_VARARGS, "" },
	{ "load_struct", py_unreal_engine_load_struct, METH_VARARGS, "" },
	{ "load_enum", py_unreal_engine_load_enum, METH_VARARGS, "" },

	{ "find_object", py_unreal_engine_find_object, METH_VARARGS, "" },
	{ "load_object", py_unreal_engine_load_object, METH_VARARGS, "" },

	{ "load_package", py_unreal_engine_load_package, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "unload_package", py_unreal_engine_unload_package, METH_VARARGS, "" },
	{ "get_package_filename", py_unreal_engine_get_package_filename, METH_VARARGS, "" },
#endif
	{ "get_forward_vector", py_unreal_engine_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", py_unreal_engine_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", py_unreal_engine_get_right_vector, METH_VARARGS, "" },

	{ "get_content_dir", py_unreal_engine_get_content_dir, METH_VARARGS, "" },
	{ "get_game_saved_dir", py_unreal_engine_get_game_saved_dir, METH_VARARGS, "" },
	{ "get_game_user_developer_dir", py_unreal_engine_get_game_user_developer_dir, METH_VARARGS, "" },
	{ "convert_relative_path_to_full", py_unreal_engine_convert_relative_path_to_full, METH_VARARGS, "" },

	{ "get_path", py_unreal_engine_get_path, METH_VARARGS, "" },
	{ "get_base_filename", py_unreal_engine_get_base_filename, METH_VARARGS, "" },
	{ "object_path_to_package_name", py_unreal_engine_object_path_to_package_name, METH_VARARGS, "" },

	{ "compress_image_array", py_unreal_engine_compress_image_array, METH_VARARGS, "" },
	{ "create_checkerboard_texture", py_unreal_engine_create_checkerboard_texture, METH_VARARGS, "" },
	{ "create_transient_texture", py_unreal_engine_create_transient_texture, METH_VARARGS, "" },
	{ "create_transient_texture_render_target2d", py_unreal_engine_create_transient_texture_render_target2d, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "create_texture", py_unreal_engine_create_texture, METH_VARARGS, "" },
#endif

	{ "create_world", py_unreal_engine_create_world, METH_VARARGS, "" },

	// package
	{ "create_package", (PyCFunction)py_unreal_engine_create_package, METH_VARARGS, "" },
	{ "get_or_create_package", (PyCFunction)py_unreal_engine_get_or_create_package, METH_VARARGS, "" },
	{ "get_transient_package", (PyCFunction)py_unreal_engine_get_transient_package, METH_VARARGS, "" },


	{ "open_file_dialog", py_unreal_engine_open_file_dialog, METH_VARARGS, "" },
	{ "save_file_dialog", py_unreal_engine_save_file_dialog, METH_VARARGS, "" },
	{ "open_directory_dialog", py_unreal_engine_open_directory_dialog, METH_VARARGS, "" },
	{ "open_font_dialog", py_unreal_engine_open_font_dialog, METH_VARARGS, "" },

	// slate

	{ "find_slate_style", py_unreal_engine_find_slate_style, METH_VARARGS, "" },
	{ "find_icon_for_class", py_unreal_engine_find_icon_for_class, METH_VARARGS, "" },

	{ "register_nomad_tab_spawner", py_unreal_engine_register_nomad_tab_spawner, METH_VARARGS, "" },
	{ "unregister_nomad_tab_spawner", py_unreal_engine_unregister_nomad_tab_spawner, METH_VARARGS, "" },
	{ "invoke_tab", py_unreal_engine_invoke_tab, METH_VARARGS, "" },
	{ "get_swidget_from_wrapper", py_unreal_engine_get_swidget_from_wrapper, METH_VARARGS, "" },
	{ "create_wrapper_from_pyswidget", py_unreal_engine_create_wrapper_from_pyswidget, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_editor_window", py_unreal_engine_get_editor_window, METH_VARARGS, "" },
	{ "add_menu_extension", py_unreal_engine_add_menu_extension, METH_VARARGS, "" },
	{ "add_menu_bar_extension", py_unreal_engine_add_menu_bar_extension, METH_VARARGS, "" },
	{ "add_tool_bar_extension", py_unreal_engine_add_tool_bar_extension, METH_VARARGS, "" },

	{ "add_asset_view_context_menu_extension", py_unreal_engine_add_asset_view_context_menu_extension, METH_VARARGS, "" },

	{ "redraw_all_viewports", py_unreal_engine_redraw_all_viewports, METH_VARARGS, "" },
	{ "update_ui", py_unreal_engine_update_ui, METH_VARARGS, "" },

#pragma warning(suppress: 4191)
	{ "create_detail_view", (PyCFunction)py_unreal_engine_create_detail_view, METH_VARARGS | METH_KEYWORDS, "" },
#pragma warning(suppress: 4191)
	{ "create_structure_detail_view", (PyCFunction)py_unreal_engine_create_structure_detail_view, METH_VARARGS | METH_KEYWORDS, "" },
#pragma warning(suppress: 4191)
	{ "create_property_view",  (PyCFunction)py_unreal_engine_create_property_view, METH_VARARGS | METH_KEYWORDS, "" },

	{ "open_editor_for_asset", py_unreal_engine_open_editor_for_asset, METH_VARARGS, "" },
	{ "find_editor_for_asset", py_unreal_engine_find_editor_for_asset, METH_VARARGS, "" },
	{ "get_all_edited_assets", py_unreal_engine_get_all_edited_assets, METH_VARARGS, "" },
	{ "close_editor_for_asset", py_unreal_engine_close_editor_for_asset, METH_VARARGS, "" },
	{ "close_all_asset_editors", py_unreal_engine_close_all_asset_editors, METH_VARARGS, "" },
	{ "allow_actor_script_execution_in_editor", py_unreal_engine_allow_actor_script_execution_in_editor , METH_VARARGS, "" },
	{ "get_editor_world", py_unreal_engine_get_editor_world, METH_VARARGS, "" },
	{ "console_exec", py_unreal_engine_console_exec, METH_VARARGS, "" },
	{ "editor_get_selected_actors", py_unreal_engine_editor_get_selected_actors, METH_VARARGS, "" },
	{ "editor_select_actor", py_unreal_engine_editor_select_actor, METH_VARARGS, "" },
	{ "editor_deselect_actors", py_unreal_engine_editor_deselect_actors, METH_VARARGS, "" },
	{ "import_asset", py_unreal_engine_import_asset, METH_VARARGS, "" },
	{ "export_assets", py_unreal_engine_export_assets, METH_VARARGS, "" },
	{ "get_asset", py_unreal_engine_get_asset, METH_VARARGS, "" },
	{ "find_asset", py_unreal_engine_find_asset, METH_VARARGS, "" },
	{ "create_asset", py_unreal_engine_create_asset, METH_VARARGS, "" },
	{ "delete_object", py_unreal_engine_delete_object, METH_VARARGS, "" },
	{ "get_assets", py_unreal_engine_get_assets, METH_VARARGS, "" },
	{ "get_selected_assets", py_unreal_engine_get_selected_assets, METH_VARARGS, "" },
	{ "get_assets_by_class", py_unreal_engine_get_assets_by_class, METH_VARARGS, "" },

	{ "is_loading_assets", py_unreal_engine_is_loading_assets, METH_VARARGS, "" },
	{ "wait_for_assets", py_unreal_engine_wait_for_assets, METH_VARARGS, "" },

	{ "sync_browser_to_assets", py_unreal_engine_editor_sync_browser_to_assets, METH_VARARGS, "" },

	{ "get_asset_referencers", py_unreal_engine_get_asset_referencers, METH_VARARGS, "" },
	{ "get_asset_identifier_referencers", py_unreal_engine_get_asset_identifier_referencers, METH_VARARGS, "" },
	{ "get_asset_dependencies", py_unreal_engine_get_asset_dependencies, METH_VARARGS, "" },

	{ "rename_asset", py_unreal_engine_rename_asset, METH_VARARGS, "" },
	{ "duplicate_asset", py_unreal_engine_duplicate_asset, METH_VARARGS, "" },
	{ "delete_asset", py_unreal_engine_delete_asset, METH_VARARGS, "" },

	{ "get_long_package_path", py_unreal_engine_get_long_package_path, METH_VARARGS, "" },
	{ "get_long_package_asset_name", py_unreal_engine_get_long_package_asset_name, METH_VARARGS, "" },

	{ "editor_command_build", py_unreal_engine_editor_command_build, METH_VARARGS, "" },
	{ "editor_command_build_lighting", py_unreal_engine_editor_command_build_lighting, METH_VARARGS, "" },
	{ "editor_command_save_current_level", py_unreal_engine_editor_command_save_current_level, METH_VARARGS, "" },
	{ "editor_command_save_all_levels", py_unreal_engine_editor_command_save_all_levels, METH_VARARGS, "" },

	{ "editor_save_all", py_unreal_engine_editor_save_all, METH_VARARGS, "" },

	{ "get_discovered_plugins", py_unreal_engine_get_discovered_plugins, METH_VARARGS, "" },
	{ "get_enabled_plugins", py_unreal_engine_get_enabled_plugins, METH_VARARGS, "" },
	{ "find_plugin", py_unreal_engine_find_plugin, METH_VARARGS, "" },

	{ "string_to_guid", py_unreal_engine_string_to_guid, METH_VARARGS, "" },
	{ "new_guid", py_unreal_engine_new_guid, METH_VARARGS, "" },
	{ "guid_to_string", py_unreal_engine_guid_to_string, METH_VARARGS, "" },

	{ "heightmap_expand", py_unreal_engine_heightmap_expand, METH_VARARGS, "" },
	{ "heightmap_import", py_unreal_engine_heightmap_import, METH_VARARGS, "" },

	{ "play_preview_sound", py_unreal_engine_play_preview_sound, METH_VARARGS, "" },

#pragma warning(suppress: 4191)
	{ "get_assets_by_filter", (PyCFunction)py_unreal_engine_get_assets_by_filter, METH_VARARGS | METH_KEYWORDS, "" },
	{ "create_blueprint", py_unreal_engine_create_blueprint, METH_VARARGS, "" },
	{ "create_blueprint_from_actor", py_unreal_engine_create_blueprint_from_actor, METH_VARARGS, "" },
	{ "replace_blueprint", py_unreal_engine_replace_blueprint, METH_VARARGS, "" },
	{ "get_blueprint_hierarchy_from_class", py_unreal_engine_get_blueprint_hierarchy_from_class, METH_VARARGS, "" },
	{ "reload_blueprint", py_unreal_engine_reload_blueprint, METH_VARARGS, "" },
	{ "compile_blueprint", py_unreal_engine_compile_blueprint, METH_VARARGS, "" },
	{ "blueprint_add_member_variable", py_unreal_engine_blueprint_add_member_variable, METH_VARARGS, "" },
	{ "blueprint_add_event_dispatcher", py_unreal_engine_blueprint_add_event_dispatcher, METH_VARARGS, "" },
	{ "blueprint_add_new_timeline", py_unreal_engine_blueprint_add_new_timeline, METH_VARARGS, "" },
	{ "blueprint_set_variable_visibility", py_unreal_engine_blueprint_set_variable_visibility, METH_VARARGS, "" },
	{ "blueprint_add_function", py_unreal_engine_blueprint_add_function, METH_VARARGS, "" },
	{ "blueprint_add_ubergraph_page", py_unreal_engine_blueprint_add_ubergraph_page, METH_VARARGS, "" },
	{ "blueprint_get_all_graphs", py_unreal_engine_blueprint_get_all_graphs, METH_VARARGS, "" },
	{ "blueprint_mark_as_structurally_modified", py_unreal_engine_blueprint_mark_as_structurally_modified, METH_VARARGS, "" },
	{ "add_component_to_blueprint", py_unreal_engine_add_component_to_blueprint, METH_VARARGS, "" },
	{ "remove_component_from_blueprint", py_unreal_engine_remove_component_from_blueprint, METH_VARARGS, "" },
	{ "get_blueprint_components", py_unreal_engine_get_blueprint_components, METH_VARARGS, "" },
	{ "create_material_instance", py_unreal_engine_create_material_instance, METH_VARARGS, "" },
	{ "message_dialog_open", py_unreal_engine_message_dialog_open, METH_VARARGS, "" },
	{ "create_modal_save_asset_dialog", py_unreal_engine_create_modal_save_asset_dialog, METH_VARARGS, "" },
	{ "set_fbx_import_option", py_unreal_engine_set_fbx_import_option, METH_VARARGS, "" },

	{ "create_new_graph", py_unreal_engine_create_new_graph, METH_VARARGS, "" },

	{ "editor_play", py_unreal_engine_editor_play, METH_VARARGS, "" },

	{ "add_level_to_world", py_unreal_engine_add_level_to_world, METH_VARARGS, "" },
	{ "move_selected_actors_to_level", py_unreal_engine_move_selected_actors_to_level, METH_VARARGS, "" },
	{ "move_actor_to_level", py_unreal_engine_move_actor_to_level, METH_VARARGS, "" },

	{ "editor_on_asset_post_import", py_unreal_engine_editor_on_asset_post_import, METH_VARARGS, "" },
	{ "on_main_frame_creation_finished", py_unreal_engine_on_main_frame_creation_finished, METH_VARARGS, "" },


	// transactions
	{ "begin_transaction", py_unreal_engine_begin_transaction, METH_VARARGS, "" },
	{ "cancel_transaction", py_unreal_engine_cancel_transaction, METH_VARARGS, "" },
	{ "end_transaction", py_unreal_engine_end_transaction, METH_VARARGS, "" },
	{ "get_transaction_name", py_unreal_engine_get_transaction_name, METH_VARARGS, "" },
	{ "is_transaction_active", py_unreal_engine_is_transaction_active, METH_VARARGS, "" },
	{ "redo_transaction", py_unreal_engine_redo_transaction, METH_VARARGS, "" },
	{ "reset_transaction", py_unreal_engine_reset_transaction, METH_VARARGS, "" },
	{ "transactions", py_unreal_engine_transactions, METH_VARARGS, "" },

	{ "editor_undo", py_unreal_engine_editor_undo, METH_VARARGS, "" },
	{ "editor_redo", py_unreal_engine_editor_redo, METH_VARARGS, "" },

	{ "editor_tick", py_unreal_engine_editor_tick, METH_VARARGS, "" },
#endif

	{ "engine_tick", py_unreal_engine_engine_tick, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "tick_rendering_tickables", py_unreal_engine_tick_rendering_tickables, METH_VARARGS, "" },
	{ "all_viewport_clients", py_unreal_engine_all_viewport_clients , METH_VARARGS, "" },
#endif
	{ "slate_tick", py_unreal_engine_slate_tick, METH_VARARGS, "" },
	{ "get_delta_time", py_unreal_engine_get_delta_time, METH_VARARGS, "" },

	{ "new_object", py_unreal_engine_new_object, METH_VARARGS, "" },


	{ "get_mutable_default", py_unreal_engine_get_mutable_default, METH_VARARGS, "" },

	{ "all_classes", (PyCFunction)py_unreal_engine_all_classes, METH_VARARGS, "" },
	{ "all_worlds", (PyCFunction)py_unreal_engine_all_worlds, METH_VARARGS, "" },
	{ "tobject_iterator", (PyCFunction)py_unreal_engine_tobject_iterator, METH_VARARGS, "" },

	{ "new_class", py_unreal_engine_new_class, METH_VARARGS, "" },


	{ "create_and_dispatch_when_ready", py_unreal_engine_create_and_dispatch_when_ready, METH_VARARGS, "" },
#if PLATFORM_MAC
	{ "main_thread_call", py_unreal_engine_main_thread_call, METH_VARARGS, "" },
#endif

	{ "add_ticker", py_unreal_engine_add_ticker, METH_VARARGS, "" },
	{ "remove_ticker", py_unreal_engine_remove_ticker, METH_VARARGS, "" },

	{ "py_gc", py_unreal_engine_py_gc, METH_VARARGS, "" },
	// exec is a reserved keyword in python2
#if PY_MAJOR_VERSION >= 3
	{ "exec", py_unreal_engine_exec, METH_VARARGS, "" },
#endif
	{ "py_exec", py_unreal_engine_exec, METH_VARARGS, "" },
#if PLATFORM_MAC
	{ "exec_in_main_thread", py_unreal_engine_exec_in_main_thread, METH_VARARGS, "" },
	{ "py_exec_in_main_thread", py_unreal_engine_exec_in_main_thread, METH_VARARGS, "" },
#endif

	{ "get_engine_defined_action_mappings", py_unreal_engine_get_engine_defined_action_mappings, METH_VARARGS, "" },

	{ "get_viewport_screenshot", py_unreal_engine_get_viewport_screenshot, METH_VARARGS, "" },
	{ "get_viewport_size", py_unreal_engine_get_viewport_size, METH_VARARGS, "" },
	{ "get_resolution", py_unreal_engine_get_resolution, METH_VARARGS, "" },
	{ "get_game_viewport_size", py_unreal_engine_get_game_viewport_size, METH_VARARGS, "" },

	{ "get_game_viewport_client", py_unreal_engine_get_game_viewport_client, METH_VARARGS, "" },
#pragma warning(suppress: 4191)
	{ "open_color_picker", (PyCFunction)py_unreal_engine_open_color_picker, METH_VARARGS | METH_KEYWORDS, "" },
	{ "destroy_color_picker", py_unreal_engine_destroy_color_picker, METH_VARARGS, "" },
	{ "play_sound", py_unreal_engine_play_sound, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "editor_play_in_viewport", py_unreal_engine_editor_play_in_viewport, METH_VARARGS, "" },
	{ "request_play_session", py_unreal_engine_request_play_session, METH_VARARGS, "" },
	{ "get_editor_pie_game_viewport_client", py_unreal_engine_get_editor_pie_game_viewport_client, METH_VARARGS, "" },
	{ "editor_get_active_viewport_screenshot", py_unreal_engine_editor_get_active_viewport_screenshot, METH_VARARGS, "" },
	{ "editor_get_pie_viewport_screenshot", py_unreal_engine_editor_get_pie_viewport_screenshot, METH_VARARGS, "" },

	{ "editor_set_view_mode", py_unreal_engine_editor_set_view_mode, METH_VARARGS, "" },
	{ "editor_set_camera_speed", py_unreal_engine_editor_set_camera_speed, METH_VARARGS, "" },
	{ "editor_set_view_location", py_unreal_engine_editor_set_view_location, METH_VARARGS, "" },
	{ "editor_set_view_rotation", py_unreal_engine_editor_set_view_rotation, METH_VARARGS, "" },

	{ "editor_get_active_viewport_size", py_unreal_engine_editor_get_active_viewport_size, METH_VARARGS, "" },
	{ "editor_get_pie_viewport_size", py_unreal_engine_editor_get_pie_viewport_size, METH_VARARGS, "" },

	{ "editor_take_high_res_screen_shots", py_unreal_engine_editor_take_high_res_screen_shots, METH_VARARGS, "" },

	{ "register_settings", py_unreal_engine_register_settings, METH_VARARGS, "" },
	{ "show_viewer", py_unreal_engine_show_viewer, METH_VARARGS, "" },
	{ "unregister_settings", py_unreal_engine_unregister_settings, METH_VARARGS, "" },

	{ "in_editor_capture", py_unreal_engine_in_editor_capture, METH_VARARGS, "" },
#endif

	{ "clipboard_copy", py_unreal_engine_clipboard_copy, METH_VARARGS, "" },
	{ "clipboard_paste", py_unreal_engine_clipboard_paste, METH_VARARGS, "" },


#pragma warning(suppress: 4191)
	{ "copy_properties_for_unrelated_objects", (PyCFunction)py_unreal_engine_copy_properties_for_unrelated_objects, METH_VARARGS | METH_KEYWORDS, "" },


	{ NULL, NULL },
};


static PyMethodDef ue_PyUObject_methods[] = {

	// Transform

	{ "get_actor_location", (PyCFunction)py_ue_get_actor_location, METH_VARARGS, "" },
	{ "get_actor_rotation", (PyCFunction)py_ue_get_actor_rotation, METH_VARARGS, "" },
	{ "get_actor_scale", (PyCFunction)py_ue_get_actor_scale, METH_VARARGS, "" },
	{ "get_actor_transform", (PyCFunction)py_ue_get_actor_transform, METH_VARARGS, "" },

	{ "get_actor_forward", (PyCFunction)py_ue_get_actor_forward, METH_VARARGS, "" },
	{ "get_actor_right", (PyCFunction)py_ue_get_actor_right, METH_VARARGS, "" },
	{ "get_actor_up", (PyCFunction)py_ue_get_actor_up, METH_VARARGS, "" },


	{ "set_actor_location", (PyCFunction)py_ue_set_actor_location, METH_VARARGS, "" },
	{ "set_actor_rotation", (PyCFunction)py_ue_set_actor_rotation, METH_VARARGS, "" },
	{ "set_actor_scale", (PyCFunction)py_ue_set_actor_scale, METH_VARARGS, "" },
	{ "set_actor_transform", (PyCFunction)py_ue_set_actor_transform, METH_VARARGS, "" },

	{ "add_actor_world_offset", (PyCFunction)py_ue_add_actor_world_offset, METH_VARARGS, "" },
	{ "add_actor_local_offset", (PyCFunction)py_ue_add_actor_local_offset, METH_VARARGS, "" },
	{ "add_actor_world_rotation", (PyCFunction)py_ue_add_actor_world_rotation, METH_VARARGS, "" },
	{ "add_actor_local_rotation", (PyCFunction)py_ue_add_actor_local_rotation, METH_VARARGS, "" },


	{ "get_world_location", (PyCFunction)py_ue_get_world_location, METH_VARARGS, "" },
	{ "get_world_rotation", (PyCFunction)py_ue_get_world_rotation, METH_VARARGS, "" },
	{ "get_world_scale", (PyCFunction)py_ue_get_world_scale, METH_VARARGS, "" },
	{ "get_world_transform", (PyCFunction)py_ue_get_world_transform, METH_VARARGS, "" },
	{ "get_relative_location", (PyCFunction)py_ue_get_relative_location, METH_VARARGS, "" },
	{ "get_relative_rotation", (PyCFunction)py_ue_get_relative_rotation, METH_VARARGS, "" },
	{ "get_relative_scale", (PyCFunction)py_ue_get_relative_scale, METH_VARARGS, "" },
	{ "get_relative_transform", (PyCFunction)py_ue_get_relative_transform, METH_VARARGS, "" },

	{ "set_world_location", (PyCFunction)py_ue_set_world_location, METH_VARARGS, "" },
	{ "set_world_rotation", (PyCFunction)py_ue_set_world_rotation, METH_VARARGS, "" },
	{ "set_world_scale", (PyCFunction)py_ue_set_world_scale, METH_VARARGS, "" },
	{ "set_world_transform", (PyCFunction)py_ue_set_world_transform, METH_VARARGS, "" },
	{ "set_relative_location", (PyCFunction)py_ue_set_relative_location, METH_VARARGS, "" },
	{ "set_relative_rotation", (PyCFunction)py_ue_set_relative_rotation, METH_VARARGS, "" },
	{ "set_relative_scale", (PyCFunction)py_ue_set_relative_scale, METH_VARARGS, "" },
	{ "set_relative_transform", (PyCFunction)py_ue_set_relative_transform, METH_VARARGS, "" },

	{ "get_forward_vector", (PyCFunction)py_ue_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", (PyCFunction)py_ue_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", (PyCFunction)py_ue_get_right_vector, METH_VARARGS, "" },

	// UObject

	{ "get_property", (PyCFunction)py_ue_get_property, METH_VARARGS, "" },
	{ "set_property", (PyCFunction)py_ue_set_property, METH_VARARGS, "" },
	{ "set_property_flags", (PyCFunction)py_ue_set_property_flags, METH_VARARGS, "" },
	{ "add_property_flags", (PyCFunction)py_ue_add_property_flags, METH_VARARGS, "" },
	{ "get_property_flags", (PyCFunction)py_ue_get_property_flags, METH_VARARGS, "" },
	{ "properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },
	{ "get_property_class", (PyCFunction)py_ue_get_property_class, METH_VARARGS, "" },
	{ "has_property", (PyCFunction)py_ue_has_property, METH_VARARGS, "" },
	{ "get_uproperty", (PyCFunction)py_ue_get_uproperty, METH_VARARGS, "" },
	{ "get_property_struct", (PyCFunction)py_ue_get_property_struct, METH_VARARGS, "" },
	{ "get_property_array_dim", (PyCFunction)py_ue_get_property_array_dim, METH_VARARGS, "" },
	{ "get_inner", (PyCFunction)py_ue_get_inner, METH_VARARGS, "" },
	{ "get_key_prop", (PyCFunction)py_ue_get_key_prop, METH_VARARGS, "" },
	{ "get_value_prop", (PyCFunction)py_ue_get_value_prop, METH_VARARGS, "" },

	{ "functions", (PyCFunction)py_ue_functions, METH_VARARGS, "" },

	{ "is_a", (PyCFunction)py_ue_is_a, METH_VARARGS, "" },
	{ "is_valid", (PyCFunction)py_ue_is_valid, METH_VARARGS, "" },
	{ "is_child_of", (PyCFunction)py_ue_is_child_of, METH_VARARGS, "" },

	{ "call", (PyCFunction)py_ue_call, METH_VARARGS, "" },
	{ "get_owner", (PyCFunction)py_ue_get_owner, METH_VARARGS, "" },

	{ "get_outer", (PyCFunction)py_ue_get_outer, METH_VARARGS, "" },
	{ "set_outer", (PyCFunction)py_ue_set_outer, METH_VARARGS, "" },
	{ "get_outermost", (PyCFunction)py_ue_get_outermost, METH_VARARGS, "" },

	{ "get_super_class", (PyCFunction)py_ue_get_super_class, METH_VARARGS, "" },

	{ "get_name", (PyCFunction)py_ue_get_name, METH_VARARGS, "" },
	{ "get_display_name", (PyCFunction)py_ue_get_display_name, METH_VARARGS, "" },
	{ "get_path_name", (PyCFunction)py_ue_get_path_name, METH_VARARGS, "" },
	{ "get_full_name", (PyCFunction)py_ue_get_full_name, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "import_custom_properties", (PyCFunction)py_ue_import_custom_properties, METH_VARARGS, "" },
#endif

#if ENGINE_MINOR_VERSION >= 15
	{ "can_modify", (PyCFunction)py_ue_can_modify, METH_VARARGS, "" },
#endif


	{ "set_name", (PyCFunction)py_ue_set_name, METH_VARARGS, "" },

	{ "bind_event", (PyCFunction)py_ue_bind_event, METH_VARARGS, "" },
	{ "unbind_event", (PyCFunction)py_ue_unbind_event, METH_VARARGS, "" },
	{ "delegate_bind_ufunction", (PyCFunction)py_ue_delegate_bind_ufunction, METH_VARARGS, "" },

	{ "get_py_proxy", (PyCFunction)py_ue_get_py_proxy, METH_VARARGS, "" },

	{ "post_edit_change", (PyCFunction)py_ue_post_edit_change, METH_VARARGS, "" },
	{ "post_edit_change_property", (PyCFunction)py_ue_post_edit_change_property, METH_VARARGS, "" },
	{ "pre_edit_change", (PyCFunction)py_ue_pre_edit_change, METH_VARARGS, "" },
	{ "modify", (PyCFunction)py_ue_modify, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_thumbnail", (PyCFunction)py_ue_get_thumbnail, METH_VARARGS, "" },
	{ "render_thumbnail", (PyCFunction)py_ue_render_thumbnail, METH_VARARGS, "" },
	{"get_metadata_tag", (PyCFunction)py_ue_get_metadata_tag, METH_VARARGS, "" },
	{"set_metadata_tag", (PyCFunction)py_ue_set_metadata_tag, METH_VARARGS, "" },
	{ "metadata_tags", (PyCFunction)py_ue_metadata_tags, METH_VARARGS, "" },
	{ "has_metadata_tag", (PyCFunction)py_ue_has_metadata_tag, METH_VARARGS, "" },
	{"remove_metadata_tag", (PyCFunction)py_ue_remove_metadata_tag, METH_VARARGS, "" },

#endif

#if WITH_EDITOR
	{ "save_config", (PyCFunction)py_ue_save_config, METH_VARARGS, "" },
	{ "get_actor_label", (PyCFunction)py_ue_get_actor_label, METH_VARARGS, "" },
	{ "set_actor_label", (PyCFunction)py_ue_set_actor_label, METH_VARARGS, "" },
	{ "set_actor_hidden_in_game", (PyCFunction)py_ue_set_actor_hidden_in_game, METH_VARARGS, "" },

	{ "get_folder_path", (PyCFunction)py_ue_get_folder_path, METH_VARARGS, "" },
	{ "set_folder_path", (PyCFunction)py_ue_set_folder_path, METH_VARARGS, "" },

	{ "world_create_folder", (PyCFunction)py_ue_world_create_folder, METH_VARARGS, "" },
	{ "world_delete_folder", (PyCFunction)py_ue_world_delete_folder, METH_VARARGS, "" },
	{ "world_rename_folder", (PyCFunction)py_ue_world_rename_folder, METH_VARARGS, "" },
	{ "world_folders", (PyCFunction)py_ue_world_folders, METH_VARARGS, "" },

	{ "get_editor_world_counterpart_actor", (PyCFunction)py_ue_get_editor_world_counterpart_actor, METH_VARARGS, "" },
	{ "component_type_registry_invalidate_class", (PyCFunction)py_ue_component_type_registry_invalidate_class, METH_VARARGS, "" },

	{ "find_actor_by_label", (PyCFunction)py_ue_find_actor_by_label, METH_VARARGS, "" },
	{ "save_package", (PyCFunction)py_ue_save_package, METH_VARARGS, "" },
	{ "duplicate", (PyCFunction)py_ue_duplicate, METH_VARARGS, "" },
	{ "asset_can_reimport", (PyCFunction)py_ue_asset_can_reimport, METH_VARARGS, "" },
	{ "asset_reimport", (PyCFunction)py_ue_asset_reimport, METH_VARARGS, "" },

	{ "factory_create_new", (PyCFunction)py_ue_factory_create_new, METH_VARARGS, "" },
	{ "factory_import_object", (PyCFunction)py_ue_factory_import_object, METH_VARARGS, "" },

	{ "graph_add_node_call_function", (PyCFunction)py_ue_graph_add_node_call_function, METH_VARARGS, "" },
	{ "graph_add_node_custom_event", (PyCFunction)py_ue_graph_add_node_custom_event, METH_VARARGS, "" },
	{ "graph_add_node_variable_get", (PyCFunction)py_ue_graph_add_node_variable_get, METH_VARARGS, "" },
	{ "graph_add_node_variable_set", (PyCFunction)py_ue_graph_add_node_variable_set, METH_VARARGS, "" },

	{ "graph_add_node", (PyCFunction)py_ue_graph_add_node, METH_VARARGS, "" },
	{ "graph_add_node_dynamic_cast", (PyCFunction)py_ue_graph_add_node_dynamic_cast, METH_VARARGS, "" },
	{ "graph_add_node_event", (PyCFunction)py_ue_graph_add_node_event, METH_VARARGS, "" },
	{ "graph_get_good_place_for_new_node", (PyCFunction)py_ue_graph_get_good_place_for_new_node, METH_VARARGS, "" },

	{ "graph_reconstruct_node", (PyCFunction)py_ue_graph_reconstruct_node, METH_VARARGS, "" },
	{ "graph_remove_node", (PyCFunction)py_ue_graph_remove_node, METH_VARARGS, "" },

	{ "node_pins", (PyCFunction)py_ue_node_pins, METH_VARARGS, "" },
	{ "node_get_title", (PyCFunction)py_ue_node_get_title, METH_VARARGS, "" },
	{ "node_find_pin", (PyCFunction)py_ue_node_find_pin, METH_VARARGS, "" },
	{ "node_create_pin", (PyCFunction)py_ue_node_create_pin, METH_VARARGS, "" },
	{ "node_pin_type_changed", (PyCFunction)py_ue_node_pin_type_changed, METH_VARARGS, "" },
	{ "node_pin_default_value_changed", (PyCFunction)py_ue_node_pin_default_value_changed, METH_VARARGS, "" },

	{ "node_function_entry_set_pure", (PyCFunction)py_ue_node_function_entry_set_pure, METH_VARARGS, "" },

	{ "node_allocate_default_pins", (PyCFunction)py_ue_node_allocate_default_pins, METH_VARARGS, "" },
	{ "node_reconstruct", (PyCFunction)py_ue_node_reconstruct, METH_VARARGS, "" },

	{ "get_material_graph", (PyCFunction)py_ue_get_material_graph, METH_VARARGS, "" },

	{ "struct_add_variable", (PyCFunction)py_ue_struct_add_variable, METH_VARARGS, "" },
	{ "struct_get_variables", (PyCFunction)py_ue_struct_get_variables, METH_VARARGS, "" },
	{ "struct_remove_variable", (PyCFunction)py_ue_struct_remove_variable, METH_VARARGS, "" },
	{ "struct_move_variable_up", (PyCFunction)py_ue_struct_move_variable_up, METH_VARARGS, "" },
	{ "struct_move_variable_down", (PyCFunction)py_ue_struct_move_variable_down, METH_VARARGS, "" },

	{ "data_table_add_row", (PyCFunction)py_ue_data_table_add_row, METH_VARARGS, "" },
	{ "data_table_remove_row", (PyCFunction)py_ue_data_table_remove_row, METH_VARARGS, "" },
	{ "data_table_rename_row", (PyCFunction)py_ue_data_table_rename_row, METH_VARARGS, "" },
	{ "data_table_as_dict", (PyCFunction)py_ue_data_table_as_dict, METH_VARARGS, "" },
	{ "data_table_as_json", (PyCFunction)py_ue_data_table_as_json, METH_VARARGS, "" },
	{ "data_table_find_row", (PyCFunction)py_ue_data_table_find_row, METH_VARARGS, "" },
	{ "data_table_get_all_rows", (PyCFunction)py_ue_data_table_get_all_rows, METH_VARARGS, "" },
#endif

	{ "export_to_file", (PyCFunction)py_ue_export_to_file, METH_VARARGS, "" },

	{ "is_rooted", (PyCFunction)py_ue_is_rooted, METH_VARARGS, "" },
	{ "add_to_root", (PyCFunction)py_ue_add_to_root, METH_VARARGS, "" },
	{ "auto_root", (PyCFunction)py_ue_auto_root, METH_VARARGS, "" },
	{ "remove_from_root", (PyCFunction)py_ue_remove_from_root, METH_VARARGS, "" },

	{ "own", (PyCFunction)py_ue_own, METH_VARARGS, "" },
	{ "disown", (PyCFunction)py_ue_disown, METH_VARARGS, "" },
	{ "is_owned", (PyCFunction)py_ue_is_owned, METH_VARARGS, "" },

	{ "find_function", (PyCFunction)py_ue_find_function, METH_VARARGS, "" },

#pragma warning(suppress: 4191)
	{ "call_function", (PyCFunction)py_ue_call_function, METH_VARARGS | METH_KEYWORDS, "" },


	{ "all_objects", (PyCFunction)py_ue_all_objects, METH_VARARGS, "" },
	{ "all_actors", (PyCFunction)py_ue_all_actors, METH_VARARGS, "" },


	// Package
	{ "package_get_filename", (PyCFunction)py_ue_package_get_filename, METH_VARARGS, "" },
	{ "package_is_dirty", (PyCFunction)py_ue_package_is_dirty, METH_VARARGS, "" },
	{ "make_unique_object_name", (PyCFunction)py_ue_package_make_unique_object_name, METH_VARARGS, "" },

#if WITH_EDITOR
	// AssetUserData
	{ "asset_import_data", (PyCFunction)py_ue_asset_import_data, METH_VARARGS, "" },
	{ "asset_import_data_set_sources", (PyCFunction)py_ue_asset_import_data_set_sources, METH_VARARGS, "" },
#endif

	// AnimSequence
	{ "anim_get_skeleton", (PyCFunction)py_ue_anim_get_skeleton, METH_VARARGS, "" },
	{ "anim_set_skeleton", (PyCFunction)py_ue_anim_set_skeleton, METH_VARARGS, "" },
	{ "get_blend_parameter", (PyCFunction)py_ue_get_blend_parameter, METH_VARARGS, "" },
	{ "set_blend_parameter", (PyCFunction)py_ue_set_blend_parameter, METH_VARARGS, "" },

	{ "get_bone_transform", (PyCFunction)py_ue_anim_get_bone_transform, METH_VARARGS, "" },
	{ "extract_bone_transform", (PyCFunction)py_ue_anim_extract_bone_transform, METH_VARARGS, "" },
	{ "extract_root_motion", (PyCFunction)py_ue_anim_extract_root_motion, METH_VARARGS, "" },

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 13
	{ "get_raw_animation_data", (PyCFunction)py_ue_anim_sequence_get_raw_animation_data, METH_VARARGS, "" },
	{ "get_raw_animation_track", (PyCFunction)py_ue_anim_sequence_get_raw_animation_track, METH_VARARGS, "" },
	{ "add_new_raw_track", (PyCFunction)py_ue_anim_sequence_add_new_raw_track, METH_VARARGS, "" },
#if ENGINE_MINOR_VERSION <23
	{ "update_compressed_track_map_from_raw", (PyCFunction)py_ue_anim_sequence_update_compressed_track_map_from_raw, METH_VARARGS, "" },
#endif
	{ "update_raw_track", (PyCFunction)py_ue_anim_sequence_update_raw_track, METH_VARARGS, "" },
	{ "apply_raw_anim_changes", (PyCFunction)py_ue_anim_sequence_apply_raw_anim_changes, METH_VARARGS, "" },
	{ "add_key_to_sequence", (PyCFunction)py_ue_anim_add_key_to_sequence, METH_VARARGS, "" },
#endif
	{ "add_anim_composite_section", (PyCFunction)py_ue_add_anim_composite_section, METH_VARARGS, "" },
#endif

	// VisualLogger
	{ "vlog", (PyCFunction)py_ue_vlog, METH_VARARGS, "" },
	{ "vlog_cylinder", (PyCFunction)py_ue_vlog_cylinder, METH_VARARGS, "" },

	// StaticMesh
	{ "get_static_mesh_bounds", (PyCFunction)py_ue_static_mesh_get_bounds, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "static_mesh_build", (PyCFunction)py_ue_static_mesh_build, METH_VARARGS, "" },
	{ "static_mesh_create_body_setup", (PyCFunction)py_ue_static_mesh_create_body_setup, METH_VARARGS, "" },
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

	{ "input_key", (PyCFunction)py_ue_input_key, METH_VARARGS, "" },
	{ "input_axis", (PyCFunction)py_ue_input_axis, METH_VARARGS, "" },

	// HUD
	{ "hud_draw_2d_line", (PyCFunction)py_ue_hud_draw_2d_line, METH_VARARGS, "" },
	{ "hud_draw_line", (PyCFunction)py_ue_hud_draw_line, METH_VARARGS, "" },
	{ "hud_draw_texture", (PyCFunction)py_ue_hud_draw_texture, METH_VARARGS, "" },
	{ "hud_draw_rect", (PyCFunction)py_ue_hud_draw_rect, METH_VARARGS, "" },
	{ "hud_draw_text", (PyCFunction)py_ue_hud_draw_text, METH_VARARGS, "" },


	// Movements

	{ "add_controller_pitch_input", (PyCFunction)py_ue_add_controller_pitch_input, METH_VARARGS, "" },
	{ "add_controller_yaw_input", (PyCFunction)py_ue_add_controller_yaw_input, METH_VARARGS, "" },
	{ "add_controller_roll_input", (PyCFunction)py_ue_add_controller_roll_input, METH_VARARGS, "" },
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
	{ "class_generated_by", (PyCFunction)py_ue_class_generated_by, METH_VARARGS, "" },
	{ "class_get_flags", (PyCFunction)py_ue_class_get_flags, METH_VARARGS, "" },
	{ "class_set_flags", (PyCFunction)py_ue_class_set_flags, METH_VARARGS, "" },
	{ "get_obj_flags", (PyCFunction)py_ue_get_obj_flags, METH_VARARGS, "" },
	{ "set_obj_flags", (PyCFunction)py_ue_set_obj_flags, METH_VARARGS, "" },
	{ "clear_obj_flags", (PyCFunction)py_ue_clear_obj_flags, METH_VARARGS, "" },
	{ "reset_obj_flags", (PyCFunction)py_ue_reset_obj_flags, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "class_get_config_name", (PyCFunction)py_ue_class_get_config_name, METH_VARARGS, "" },
	{ "class_set_config_name", (PyCFunction)py_ue_class_set_config_name, METH_VARARGS, "" },
#endif
	{ "get_actor_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },
	{ "get_components", (PyCFunction)py_ue_actor_components, METH_VARARGS, "" },

	{ "component_is_registered", (PyCFunction)py_ue_component_is_registered, METH_VARARGS, "" },
	{ "register_component", (PyCFunction)py_ue_register_component, METH_VARARGS, "" },
	{ "unregister_component", (PyCFunction)py_ue_unregister_component, METH_VARARGS, "" },
	{ "destroy_component", (PyCFunction)py_ue_destroy_component, METH_VARARGS, "" },

	{ "actor_destroy_component", (PyCFunction)py_ue_actor_destroy_component, METH_VARARGS, "" },
	{ "destroy_actor_component", (PyCFunction)py_ue_actor_destroy_component, METH_VARARGS, "" },

	{ "actor_create_default_subobject", (PyCFunction)py_ue_actor_create_default_subobject, METH_VARARGS, "" },
	{ "create_default_subobject", (PyCFunction)py_ue_actor_create_default_subobject, METH_VARARGS, "" },

	{ "actor_begin_play", (PyCFunction)py_ue_actor_begin_play, METH_VARARGS, "" },

	{ "broadcast", (PyCFunction)py_ue_broadcast, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_metadata", (PyCFunction)py_ue_get_metadata, METH_VARARGS, "" },
	{ "set_metadata", (PyCFunction)py_ue_set_metadata, METH_VARARGS, "" },
	{ "has_metadata", (PyCFunction)py_ue_has_metadata, METH_VARARGS, "" },
#endif


	{ "quit_game", (PyCFunction)py_ue_quit_game, METH_VARARGS, "" },
	{ "play", (PyCFunction)py_ue_play, METH_VARARGS, "" },

	{ "get_world_type", (PyCFunction)py_ue_get_world_type, METH_VARARGS, "" },

	{ "world_exec", (PyCFunction)py_ue_world_exec, METH_VARARGS, "" },

	{ "simple_move_to_location", (PyCFunction)py_ue_simple_move_to_location, METH_VARARGS, "" },

	{ "actor_has_component_of_type", (PyCFunction)py_ue_actor_has_component_of_type, METH_VARARGS, "" },

	{ "actor_destroy", (PyCFunction)py_ue_actor_destroy, METH_VARARGS, "" },
#pragma warning(suppress: 4191)
	{ "actor_spawn", (PyCFunction)py_ue_actor_spawn, METH_VARARGS | METH_KEYWORDS, "" },
	{ "actor_has_tag", (PyCFunction)py_ue_actor_has_tag, METH_VARARGS, "" },
	{ "component_has_tag", (PyCFunction)py_ue_component_has_tag, METH_VARARGS, "" },
	{ "get_actor_bounds", (PyCFunction)py_ue_get_actor_bounds, METH_VARARGS, "" },

	{ "line_trace_single_by_channel", (PyCFunction)py_ue_line_trace_single_by_channel, METH_VARARGS, "" },
	{ "line_trace_multi_by_channel", (PyCFunction)py_ue_line_trace_multi_by_channel, METH_VARARGS, "" },
	{ "get_hit_result_under_cursor", (PyCFunction)py_ue_get_hit_result_under_cursor, METH_VARARGS, "" },
	{ "draw_debug_line", (PyCFunction)py_ue_draw_debug_line, METH_VARARGS, "" },

	{ "destructible_apply_damage", (PyCFunction)py_ue_destructible_apply_damage, METH_VARARGS, "" },

	{ "set_view_target", (PyCFunction)py_ue_set_view_target, METH_VARARGS, "" },
	{ "get_world_delta_seconds", (PyCFunction)py_ue_get_world_delta_seconds, METH_VARARGS, "" },

	{ "get_levels", (PyCFunction)py_ue_get_levels, METH_VARARGS, "" },
	{ "get_current_level", (PyCFunction)py_ue_get_current_level, METH_VARARGS, "" },
	{ "set_current_level", (PyCFunction)py_ue_set_current_level, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_level_script_blueprint", (PyCFunction)py_ue_get_level_script_blueprint, METH_VARARGS, "" },
	{ "add_foliage_asset", (PyCFunction)py_ue_add_foliage_asset, METH_VARARGS, "" },
	{ "get_foliage_instances", (PyCFunction)py_ue_get_foliage_instances, METH_VARARGS, "" },
#endif
	{ "get_instanced_foliage_actor_for_current_level", (PyCFunction)py_ue_get_instanced_foliage_actor_for_current_level, METH_VARARGS, "" },
	{ "get_instanced_foliage_actor_for_level", (PyCFunction)py_ue_get_instanced_foliage_actor_for_level, METH_VARARGS, "" },
	{ "get_foliage_types", (PyCFunction)py_ue_get_foliage_types, METH_VARARGS, "" },


	{ "add_actor_component", (PyCFunction)py_ue_add_actor_component, METH_VARARGS, "" },
	{ "add_instance_component", (PyCFunction)py_ue_add_instance_component, METH_VARARGS, "" },
	{ "get_actor_root_component", (PyCFunction)py_ue_get_actor_root_component, METH_VARARGS, "" },
	{ "add_actor_root_component", (PyCFunction)py_ue_add_actor_root_component, METH_VARARGS, "" },
	{ "get_actor_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_actor_component_by_class", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component_by_class", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_actor_components_by_class", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_components_by_class", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_actor_components_by_tag", (PyCFunction)py_ue_get_actor_components_by_tag, METH_VARARGS, "" },
	{ "get_components_by_tag", (PyCFunction)py_ue_get_actor_components_by_tag, METH_VARARGS, "" },

	{ "add_python_component", (PyCFunction)py_ue_add_python_component, METH_VARARGS, "" },

	{ "set_simulate_physics", (PyCFunction)py_ue_set_simulate_physics, METH_VARARGS, "" },
	{ "add_impulse", (PyCFunction)py_ue_add_impulse, METH_VARARGS, "" },
	{ "add_angular_impulse", (PyCFunction)py_ue_add_angular_impulse, METH_VARARGS, "" },
	{ "add_force", (PyCFunction)py_ue_add_force, METH_VARARGS, "" },
	{ "add_torque", (PyCFunction)py_ue_add_torque, METH_VARARGS, "" },
	{ "set_physics_linear_velocity", (PyCFunction)py_ue_set_physics_linear_velocity, METH_VARARGS, "" },
	{ "get_physics_linear_velocity", (PyCFunction)py_ue_get_physics_linear_velocity, METH_VARARGS, "" },
	{ "set_physics_angular_velocity", (PyCFunction)py_ue_set_physics_angular_velocity, METH_VARARGS, "" },
	{ "get_physics_angular_velocity", (PyCFunction)py_ue_get_physics_angular_velocity, METH_VARARGS, "" },
	{ "find_object", (PyCFunction)py_ue_find_object, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_get_world, METH_VARARGS, "" },
	{ "has_world", (PyCFunction)py_ue_has_world, METH_VARARGS, "" },

	{ "get_game_viewport", (PyCFunction)py_ue_get_game_viewport, METH_VARARGS, "" },

	{ "game_viewport_client_set_rendering_flag", (PyCFunction)py_ue_game_viewport_client_set_rendering_flag, METH_VARARGS, "" },

	{ "get_world_location_at_distance_along_spline", (PyCFunction)py_ue_get_world_location_at_distance_along_spline, METH_VARARGS, "" },
	{ "get_spline_length", (PyCFunction)py_ue_get_spline_length, METH_VARARGS, "" },

	{ "game_viewport_client_get_window", (PyCFunction)py_ue_game_viewport_client_get_window, METH_VARARGS, "" },

	// Widget
	{ "take_widget", (PyCFunction)py_ue_take_widget, METH_VARARGS, "" },
	{ "create_widget", (PyCFunction)py_ue_create_widget, METH_VARARGS, "" },

	// WidgetComponent
	{ "set_slate_widget", (PyCFunction)py_ue_set_slate_widget, METH_VARARGS, "" },

	{ "get_actor_velocity", (PyCFunction)py_ue_get_actor_velocity, METH_VARARGS, "" },

	{ "play_sound_at_location", (PyCFunction)py_ue_play_sound_at_location, METH_VARARGS, "" },
	{ "queue_audio", (PyCFunction)py_ue_queue_audio, METH_VARARGS, "" },
	{ "reset_audio", (PyCFunction)py_ue_reset_audio, METH_VARARGS, "" },
	{ "get_available_audio_byte_count", (PyCFunction)py_ue_get_available_audio_byte_count, METH_VARARGS, "" },
	{ "sound_get_data", (PyCFunction)py_ue_sound_get_data, METH_VARARGS, "" },
	{ "sound_set_data", (PyCFunction)py_ue_sound_set_data, METH_VARARGS, "" },

	{ "world_tick", (PyCFunction)py_ue_world_tick, METH_VARARGS, "" },

	{ "conditional_begin_destroy", (PyCFunction)py_ue_conditional_begin_destroy, METH_VARARGS, "" },

	// Landscape
#if WITH_EDITOR
	{ "create_landscape_info", (PyCFunction)py_ue_create_landscape_info, METH_VARARGS, "" },
	{ "get_landscape_info", (PyCFunction)py_ue_get_landscape_info, METH_VARARGS, "" },
	{ "landscape_import", (PyCFunction)py_ue_landscape_import, METH_VARARGS, "" },
	{ "landscape_export_to_raw_mesh", (PyCFunction)py_ue_landscape_export_to_raw_mesh, METH_VARARGS, "" },
#endif

	// Player

	{ "create_player", (PyCFunction)py_ue_create_player, METH_VARARGS, "" },
	{ "get_num_players", (PyCFunction)py_ue_get_num_players, METH_VARARGS, "" },
	{ "get_num_spectators", (PyCFunction)py_ue_get_num_spectators, METH_VARARGS, "" },
	{ "get_player_controller", (PyCFunction)py_ue_get_player_controller, METH_VARARGS, "" },
	{ "get_player_hud", (PyCFunction)py_ue_get_player_hud, METH_VARARGS, "" },
	{ "set_player_hud", (PyCFunction)py_ue_set_player_hud, METH_VARARGS, "" },
	{ "get_player_camera_manager", (PyCFunction)py_ue_get_player_camera_manager, METH_VARARGS, "" },
	{ "get_player_pawn", (PyCFunction)py_ue_get_player_pawn, METH_VARARGS, "" },
	{ "restart_level", (PyCFunction)py_ue_restart_level, METH_VARARGS, "" },

	{ "get_overlapping_actors", (PyCFunction)py_ue_get_overlapping_actors, METH_VARARGS, "" },
	{ "actor_set_level_sequence", (PyCFunction)py_ue_actor_set_level_sequence, METH_VARARGS, "" },

	// MovieSceneCapture
	{ "capture_initialize", (PyCFunction)py_ue_capture_initialize, METH_VARARGS, "" },
	{ "capture_start", (PyCFunction)py_ue_capture_start, METH_VARARGS, "" },
	{ "capture_stop", (PyCFunction)py_ue_capture_stop, METH_VARARGS, "" },
	{ "capture_load_from_config", (PyCFunction)py_ue_capture_load_from_config, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "set_level_sequence_asset", (PyCFunction)py_ue_set_level_sequence_asset, METH_VARARGS, "" },
#endif

	// Pawn
	{ "get_controller", (PyCFunction)py_ue_pawn_get_controller, METH_VARARGS, "" },

	// Controller
	{ "posses", (PyCFunction)py_ue_controller_posses, METH_VARARGS, "" },
	{ "unposses", (PyCFunction)py_ue_controller_unposses, METH_VARARGS, "" },
	{ "get_hud", (PyCFunction)py_ue_controller_get_hud, METH_VARARGS, "" },
	{ "get_controlled_pawn", (PyCFunction)py_ue_get_controlled_pawn, METH_VARARGS, "" },
	{ "get_pawn", (PyCFunction)py_ue_get_controlled_pawn, METH_VARARGS, "" },
	{ "project_world_location_to_screen", (PyCFunction)py_ue_controller_project_world_location_to_screen, METH_VARARGS, "" },

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
	{ "set_skeletal_mesh", (PyCFunction)py_ue_set_skeletal_mesh, METH_VARARGS, "" },

	{ "skeleton_get_parent_index", (PyCFunction)py_ue_skeleton_get_parent_index, METH_VARARGS, "" },
	{ "skeleton_bones_get_num", (PyCFunction)py_ue_skeleton_bones_get_num, METH_VARARGS, "" },
	{ "skeleton_get_bone_name", (PyCFunction)py_ue_skeleton_get_bone_name, METH_VARARGS, "" },
	{ "skeleton_find_bone_index", (PyCFunction)py_ue_skeleton_find_bone_index, METH_VARARGS, "" },
	{ "skeleton_get_ref_bone_pose", (PyCFunction)py_ue_skeleton_get_ref_bone_pose, METH_VARARGS, "" },

#if ENGINE_MINOR_VERSION > 13
	{ "skeleton_add_bone", (PyCFunction)py_ue_skeleton_add_bone, METH_VARARGS, "" },
#endif

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12
	{ "skeletal_mesh_set_soft_vertices", (PyCFunction)py_ue_skeletal_mesh_set_soft_vertices, METH_VARARGS, "" },
	{ "skeletal_mesh_get_soft_vertices", (PyCFunction)py_ue_skeletal_mesh_get_soft_vertices, METH_VARARGS, "" },
#endif
	{ "skeletal_mesh_get_lod", (PyCFunction)py_ue_skeletal_mesh_get_lod, METH_VARARGS, "" },

	{ "skeletal_mesh_get_raw_indices", (PyCFunction)py_ue_skeletal_mesh_get_raw_indices, METH_VARARGS, "" },
#endif
	{ "skeletal_mesh_set_skeleton", (PyCFunction)py_ue_skeletal_mesh_set_skeleton, METH_VARARGS, "" },

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 12
	{ "skeletal_mesh_get_bone_map", (PyCFunction)py_ue_skeletal_mesh_get_bone_map, METH_VARARGS, "" },
	{ "skeletal_mesh_set_bone_map", (PyCFunction)py_ue_skeletal_mesh_set_bone_map, METH_VARARGS, "" },
#endif
	{ "skeletal_mesh_set_active_bone_indices", (PyCFunction)py_ue_skeletal_mesh_set_active_bone_indices, METH_VARARGS, "" },
	{ "skeletal_mesh_set_required_bones", (PyCFunction)py_ue_skeletal_mesh_set_required_bones, METH_VARARGS, "" },
	{ "skeletal_mesh_get_active_bone_indices", (PyCFunction)py_ue_skeletal_mesh_get_active_bone_indices, METH_VARARGS, "" },
	{ "skeletal_mesh_get_required_bones", (PyCFunction)py_ue_skeletal_mesh_get_required_bones, METH_VARARGS, "" },
	{ "skeletal_mesh_lods_num", (PyCFunction)py_ue_skeletal_mesh_lods_num, METH_VARARGS, "" },
	{ "skeletal_mesh_sections_num", (PyCFunction)py_ue_skeletal_mesh_sections_num, METH_VARARGS, "" },

#pragma warning(suppress: 4191)
	{ "skeletal_mesh_build_lod", (PyCFunction)py_ue_skeletal_mesh_build_lod, METH_VARARGS | METH_KEYWORDS, "" },
#endif
#if WITH_EDITOR
	{ "skeletal_mesh_register_morph_target", (PyCFunction)py_ue_skeletal_mesh_register_morph_target, METH_VARARGS, "" },


	{ "skeletal_mesh_to_import_vertex_map", (PyCFunction)py_ue_skeletal_mesh_to_import_vertex_map, METH_VARARGS, "" },

	{ "morph_target_populate_deltas", (PyCFunction)py_ue_morph_target_populate_deltas, METH_VARARGS, "" },
	{ "morph_target_get_deltas", (PyCFunction)py_ue_morph_target_get_deltas, METH_VARARGS, "" },
#endif
	// Timer
	{ "set_timer", (PyCFunction)py_ue_set_timer, METH_VARARGS, "" },

	// Texture
	{ "texture_get_data", (PyCFunction)py_ue_texture_get_data, METH_VARARGS, "" },
	{ "texture_set_data", (PyCFunction)py_ue_texture_set_data, METH_VARARGS, "" },
	{ "texture_get_width", (PyCFunction)py_ue_texture_get_width, METH_VARARGS, "" },
	{ "texture_get_height", (PyCFunction)py_ue_texture_get_height, METH_VARARGS, "" },
	{ "texture_has_alpha_channel", (PyCFunction)py_ue_texture_has_alpha_channel, METH_VARARGS, "" },
	{ "render_target_get_data", (PyCFunction)py_ue_render_target_get_data, METH_VARARGS, "" },
	{ "render_target_get_data_to_buffer", (PyCFunction)py_ue_render_target_get_data_to_buffer, METH_VARARGS, "" },
	{ "texture_update_resource", (PyCFunction)py_ue_texture_update_resource, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "texture_get_source_data", (PyCFunction)py_ue_texture_get_source_data, METH_VARARGS, "" },
	{ "texture_set_source_data", (PyCFunction)py_ue_texture_set_source_data, METH_VARARGS, "" },
#endif

	// Sequencer
	{ "sequencer_master_tracks", (PyCFunction)py_ue_sequencer_master_tracks, METH_VARARGS, "" },
	{ "sequencer_possessable_tracks", (PyCFunction)py_ue_sequencer_possessable_tracks, METH_VARARGS, "" },
	{ "sequencer_get_camera_cut_track", (PyCFunction)py_ue_sequencer_get_camera_cut_track, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "sequencer_set_playback_range", (PyCFunction)py_ue_sequencer_set_playback_range, METH_VARARGS, "" },
	{ "sequencer_set_view_range", (PyCFunction)py_ue_sequencer_set_view_range, METH_VARARGS, "" },
	{ "sequencer_set_working_range", (PyCFunction)py_ue_sequencer_set_working_range, METH_VARARGS, "" },
	{ "sequencer_set_section_range", (PyCFunction)py_ue_sequencer_set_section_range, METH_VARARGS, "" },
	{ "sequencer_get_playback_range", (PyCFunction)py_ue_sequencer_get_playback_range, METH_VARARGS, "" },
	{ "sequencer_get_selection_range", (PyCFunction)py_ue_sequencer_get_selection_range, METH_VARARGS, "" },
	{ "sequencer_folders", (PyCFunction)py_ue_sequencer_folders, METH_VARARGS, "" },
	{ "sequencer_create_folder", (PyCFunction)py_ue_sequencer_create_folder, METH_VARARGS, "" },
	{ "sequencer_set_display_name", (PyCFunction)py_ue_sequencer_set_display_name, METH_VARARGS, "" },
	{ "sequencer_get_display_name", (PyCFunction)py_ue_sequencer_get_display_name, METH_VARARGS, "" },
	{ "sequencer_changed", (PyCFunction)py_ue_sequencer_changed, METH_VARARGS, "" },
	{ "sequencer_add_camera_cut_track", (PyCFunction)py_ue_sequencer_add_camera_cut_track, METH_VARARGS, "" },
	{ "sequencer_add_actor", (PyCFunction)py_ue_sequencer_add_actor, METH_VARARGS, "" },
	{ "sequencer_add_actor_component", (PyCFunction)py_ue_sequencer_add_actor_component, METH_VARARGS, "" },
	{ "sequencer_make_new_spawnable", (PyCFunction)py_ue_sequencer_make_new_spawnable, METH_VARARGS, "" },

	{ "sequencer_add_possessable", (PyCFunction)py_ue_sequencer_add_possessable, METH_VARARGS, "" },
	{ "sequencer_track_add_section", (PyCFunction)py_ue_sequencer_track_add_section, METH_VARARGS, "" },

	{ "sequencer_section_add_key", (PyCFunction)py_ue_sequencer_section_add_key, METH_VARARGS, "" },
	{ "sequencer_remove_possessable", (PyCFunction)py_ue_sequencer_remove_possessable, METH_VARARGS, "" },
	{ "sequencer_remove_spawnable", (PyCFunction)py_ue_sequencer_remove_spawnable, METH_VARARGS, "" },
	{ "sequencer_remove_camera_cut_track", (PyCFunction)py_ue_sequencer_remove_camera_cut_track, METH_VARARGS, "" },
	{ "sequencer_remove_master_track", (PyCFunction)py_ue_sequencer_remove_master_track, METH_VARARGS, "" },
	{ "sequencer_remove_track", (PyCFunction)py_ue_sequencer_remove_track, METH_VARARGS, "" },
	{ "sequencer_import_fbx_transform", (PyCFunction)py_ue_sequencer_import_fbx_transform, METH_VARARGS, "" },

#endif
	{ "sequencer_sections", (PyCFunction)py_ue_sequencer_sections, METH_VARARGS, "" },
	{ "sequencer_track_sections", (PyCFunction)py_ue_sequencer_track_sections, METH_VARARGS, "" },
	{ "sequencer_possessables", (PyCFunction)py_ue_sequencer_possessables, METH_VARARGS, "" },
	{ "sequencer_possessables_guid", (PyCFunction)py_ue_sequencer_possessables_guid, METH_VARARGS, "" },
	{ "sequencer_find_possessable", (PyCFunction)py_ue_sequencer_find_possessable, METH_VARARGS, "" },
	{ "sequencer_find_spawnable", (PyCFunction)py_ue_sequencer_find_spawnable, METH_VARARGS, "" },
	{ "sequencer_add_master_track", (PyCFunction)py_ue_sequencer_add_master_track, METH_VARARGS, "" },


	{ "sequencer_add_track", (PyCFunction)py_ue_sequencer_add_track, METH_VARARGS, "" },


	// Material
	{ "set_material", (PyCFunction)py_ue_set_material, METH_VARARGS, "" },
	{ "set_material_by_name", (PyCFunction)py_ue_set_material_by_name, METH_VARARGS, "" },
	{ "set_material_scalar_parameter", (PyCFunction)py_ue_set_material_scalar_parameter, METH_VARARGS, "" },
	{ "set_material_static_switch_parameter", (PyCFunction)py_ue_set_material_static_switch_parameter, METH_VARARGS, "" },
	{ "set_material_vector_parameter", (PyCFunction)py_ue_set_material_vector_parameter, METH_VARARGS, "" },
	{ "set_material_texture_parameter", (PyCFunction)py_ue_set_material_texture_parameter, METH_VARARGS, "" },
	{ "get_material_scalar_parameter", (PyCFunction)py_ue_get_material_scalar_parameter, METH_VARARGS, "" },
	{ "get_material_vector_parameter", (PyCFunction)py_ue_get_material_vector_parameter, METH_VARARGS, "" },
	{ "get_material_texture_parameter", (PyCFunction)py_ue_get_material_texture_parameter, METH_VARARGS, "" },
	{ "get_material_static_switch_parameter", (PyCFunction)py_ue_get_material_static_switch_parameter, METH_VARARGS, "" },
	{ "create_material_instance_dynamic", (PyCFunction)py_ue_create_material_instance_dynamic, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "set_material_parent", (PyCFunction)py_ue_set_material_parent, METH_VARARGS, "" },
	{ "static_mesh_set_collision_for_lod", (PyCFunction)py_ue_static_mesh_set_collision_for_lod, METH_VARARGS, "" },
	{ "static_mesh_set_shadow_for_lod", (PyCFunction)py_ue_static_mesh_set_shadow_for_lod, METH_VARARGS, "" },
	{ "get_raw_mesh", (PyCFunction)py_ue_static_mesh_get_raw_mesh, METH_VARARGS, "" },

	{ "static_mesh_generate_kdop10x", (PyCFunction)py_ue_static_mesh_generate_kdop10x, METH_VARARGS, "" },
	{ "static_mesh_generate_kdop10y", (PyCFunction)py_ue_static_mesh_generate_kdop10y, METH_VARARGS, "" },
	{ "static_mesh_generate_kdop10z", (PyCFunction)py_ue_static_mesh_generate_kdop10z, METH_VARARGS, "" },
	{ "static_mesh_generate_kdop18", (PyCFunction)py_ue_static_mesh_generate_kdop18, METH_VARARGS, "" },
	{ "static_mesh_generate_kdop26", (PyCFunction)py_ue_static_mesh_generate_kdop26, METH_VARARGS, "" },

	{ "static_mesh_import_lod", (PyCFunction)py_ue_static_mesh_import_lod, METH_VARARGS, "" },

#endif

	// Viewport
	{ "add_viewport_widget_content", (PyCFunction)py_ue_add_viewport_widget_content, METH_VARARGS, "" },
	{ "remove_viewport_widget_content", (PyCFunction)py_ue_remove_viewport_widget_content, METH_VARARGS, "" },
	{ "remove_all_viewport_widgets", (PyCFunction)py_ue_remove_all_viewport_widgets, METH_VARARGS, "" },

#if PY_MAJOR_VERSION >= 3
	{ "add_function", (PyCFunction)py_ue_add_function, METH_VARARGS, "" },
#endif
	{ "add_property", (PyCFunction)py_ue_add_property, METH_VARARGS, "" },
	{ "as_dict", (PyCFunction)py_ue_as_dict, METH_VARARGS, "" },

	{ "get_cdo", (PyCFunction)py_ue_get_cdo, METH_VARARGS, "" },
	{ "get_archetype", (PyCFunction)py_ue_get_archetype, METH_VARARGS, "" },
	{ "get_archetype_instances", (PyCFunction)py_ue_get_archetype_instances, METH_VARARGS, "" },
	{ "enum_values", (PyCFunction)py_ue_enum_values, METH_VARARGS, "" },
	{ "enum_names", (PyCFunction)py_ue_enum_names, METH_VARARGS, "" },
#if ENGINE_MINOR_VERSION >= 15
	{ "enum_user_defined_names", (PyCFunction)py_ue_enum_user_defined_names, METH_VARARGS, "" },
#endif

	// serialization
	{ "to_bytes", (PyCFunction)py_ue_to_bytes, METH_VARARGS, "" },
	{ "to_bytearray", (PyCFunction)py_ue_to_bytearray, METH_VARARGS, "" },
	{ "from_bytes", (PyCFunction)py_ue_from_bytes, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


// destructor
static void ue_pyobject_dealloc(ue_PyUObject* self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyUObject %p mapped to UObject %p"), self, self->ue_object);
#endif
	if (self->owned)
	{
		FUnrealEnginePythonHouseKeeper::Get()->UntrackUObject(self->ue_object);
	}

	if (self->auto_rooted && (self->ue_object && self->ue_object->IsValidLowLevel() && self->ue_object->IsRooted()))
	{
		self->ue_object->RemoveFromRoot();
	}

	Py_XDECREF(self->py_dict);

	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* ue_PyUObject_getattro(ue_PyUObject* self, PyObject* attr_name)
{
	ue_py_check(self);

	PyObject* ret = PyObject_GenericGetAttr((PyObject*)self, attr_name);
	if (!ret)
	{
		if (PyUnicodeOrString_Check(attr_name))
		{
			const char* attr = UEPyUnicode_AsUTF8(attr_name);
			// first check for property
			UStruct* u_struct = nullptr;
			if (self->ue_object->IsA<UStruct>())
			{
				u_struct = (UStruct*)self->ue_object;
			}
			else
			{
				u_struct = (UStruct*)self->ue_object->GetClass();
			}
			UProperty* u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(attr)));
			if (u_property)
			{
				// swallow previous exception
				PyErr_Clear();
				return ue_py_convert_property(u_property, (uint8*)self->ue_object, 0);
			}

			UFunction* function = self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(attr)));
			// retry wth K2_ prefix
			if (!function)
			{
				FString k2_name = FString("K2_") + UTF8_TO_TCHAR(attr);
				function = self->ue_object->FindFunction(FName(*k2_name));
			}

			// is it a static class ?
			if (!function)
			{
				if (self->ue_object->IsA<UClass>())
				{
					UClass* u_class = (UClass*)self->ue_object;
					UObject* cdo = u_class->GetDefaultObject();
					if (cdo)
					{
						function = cdo->FindFunction(FName(UTF8_TO_TCHAR(attr)));
						// try _NEW ?
						if (!function)
						{
							FString name_new = UTF8_TO_TCHAR(attr) + FString("_NEW");
							function = cdo->FindFunction(FName(*name_new));
						}
					}
				}
			}

			// last hope, is it an enum ?
			if (!function)
			{
#if ENGINE_MINOR_VERSION >= 15
				if (self->ue_object->IsA<UUserDefinedEnum>())
				{
					UUserDefinedEnum* u_enum = (UUserDefinedEnum*)self->ue_object;
					PyErr_Clear();
					FString attr_as_string = FString(UTF8_TO_TCHAR(attr));
					for (auto item : u_enum->DisplayNameMap)
					{
						if (item.Value.ToString() == attr_as_string)
						{
#if ENGINE_MINOR_VERSION > 15
							return PyLong_FromLong(u_enum->GetIndexByName(item.Key));
#else
							return PyLong_FromLong(u_enum->FindEnumIndex(item.Key));
#endif
						}
					}
					return PyErr_Format(PyExc_Exception, "unknown enum name \"%s\"", attr);
				}
#endif
				if (self->ue_object->IsA<UEnum>())
				{
					UEnum* u_enum = (UEnum*)self->ue_object;
					PyErr_Clear();
#if ENGINE_MINOR_VERSION > 15
					int32 value = u_enum->GetIndexByName(FName(UTF8_TO_TCHAR(attr)));
					if (value == INDEX_NONE)
						return PyErr_Format(PyExc_Exception, "unknown enum name \"%s\"", attr);
					return PyLong_FromLong(value);
#else
					int32 value = u_enum->FindEnumIndex(FName(UTF8_TO_TCHAR(attr)));
					if (value == INDEX_NONE)
						return PyErr_Format(PyExc_Exception, "unknown enum name \"%s\"", attr);
					return PyLong_FromLong(value);
#endif
				}
			}

			if (function)
			{
				// swallow previous exception
				PyErr_Clear();
				return py_ue_new_callable(function, self->ue_object);
			}
		}
	}
	return ret;
}

static int ue_PyUObject_setattro(ue_PyUObject* self, PyObject* attr_name, PyObject* value)
{
	ue_py_check_int(self);

	// first of all check for UProperty
	if (PyUnicodeOrString_Check(attr_name))
	{
		const char* attr = UEPyUnicode_AsUTF8(attr_name);
		// first check for property
		UStruct* u_struct = nullptr;
		if (self->ue_object->IsA<UStruct>())
		{
			u_struct = (UStruct*)self->ue_object;
		}
		else
		{
			u_struct = (UStruct*)self->ue_object->GetClass();
		}
		UProperty* u_property = u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(attr)));
		if (u_property)
		{
#if WITH_EDITOR
			self->ue_object->PreEditChange(u_property);
#endif
			if (ue_py_convert_pyobject(value, u_property, (uint8*)self->ue_object, 0))
			{
#if WITH_EDITOR
				FPropertyChangedEvent PropertyEvent(u_property, EPropertyChangeType::ValueSet);
				self->ue_object->PostEditChangeProperty(PropertyEvent);

				if (self->ue_object->HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
				{
					TArray<UObject*> Instances;
					self->ue_object->GetArchetypeInstances(Instances);
					for (UObject* Instance : Instances)
					{
						Instance->PreEditChange(u_property);
						if (ue_py_convert_pyobject(value, u_property, (uint8*)Instance, 0))
						{
							FPropertyChangedEvent InstancePropertyEvent(u_property, EPropertyChangeType::ValueSet);
							Instance->PostEditChangeProperty(InstancePropertyEvent);
						}
						else
						{
							PyErr_SetString(PyExc_ValueError, "invalid value for UProperty");
							return -1;
						}
					}
				}
#endif
				return 0;
			}
			PyErr_SetString(PyExc_ValueError, "invalid value for UProperty");
			return -1;
		}

		// now check for function name
		if (self->ue_object->FindFunction(FName(UTF8_TO_TCHAR(attr))))
		{
			PyErr_SetString(PyExc_ValueError, "you cannot overwrite a UFunction");
			return -1;
		}
	}
	return PyObject_GenericSetAttr((PyObject*)self, attr_name, value);
}

static PyObject* ue_PyUObject_str(ue_PyUObject* self)
{
	ue_py_check(self);

#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromFormat("<unreal_engine.UObject '%s' (%p) UClass '%s' (refcnt: %d)>",
		TCHAR_TO_UTF8(*self->ue_object->GetName()), self->ue_object, TCHAR_TO_UTF8(*self->ue_object->GetClass()->GetName()), self->ob_base.ob_refcnt);
#else
	return PyUnicode_FromFormat("<unreal_engine.UObject '%s' (%p) UClass '%s'>",
		TCHAR_TO_UTF8(*self->ue_object->GetName()), self->ue_object, TCHAR_TO_UTF8(*self->ue_object->GetClass()->GetName()));
#endif
}

static PyObject* ue_PyUObject_call(ue_PyUObject* self, PyObject* args, PyObject* kw)
{
	ue_py_check(self);
	// if it is a class, create a new object
	if (self->ue_object->IsA<UClass>())
	{
		UClass* u_class = (UClass*)self->ue_object;
		if (u_class->HasAnyClassFlags(CLASS_Abstract))
		{
			return PyErr_Format(PyExc_Exception, "abstract classes cannot be instantiated");
		}
		if (u_class->IsChildOf<AActor>())
		{
			return PyErr_Format(PyExc_Exception, "you cannot use __call__ on actors, they have to be spawned");
		}
		PyObject* py_name = nullptr;
		PyObject* py_outer = Py_None;
		if (!PyArg_ParseTuple(args, "|OO:new_object", &py_name, &py_outer))
		{
			return NULL;
		}
		int num_args = py_name ? 3 : 1;
		PyObject* py_args = PyTuple_New(num_args);
		Py_INCREF((PyObject*)self);
		PyTuple_SetItem(py_args, 0, (PyObject*)self);
		if (py_name)
		{
			Py_INCREF(py_outer);
			PyTuple_SetItem(py_args, 1, py_outer);
			Py_INCREF(py_name);
			PyTuple_SetItem(py_args, 2, py_name);
		}
		ue_PyUObject* ret = (ue_PyUObject*)py_unreal_engine_new_object(nullptr, py_args);
		Py_DECREF(py_args);
		if (!ret)
		{
			return NULL;
		}
		// when new_object is called the reference counting is 2 and is registered in the GC
		// UObject crated explicitely from python, will be managed by python...
		FUnrealEnginePythonHouseKeeper::Get()->TrackUObject(ret->ue_object);

		return (PyObject*)ret;
	}
	// if it is a uscriptstruct, instantiate a new struct
	if (self->ue_object->IsA<UScriptStruct>())
	{
		UScriptStruct* u_script_struct = (UScriptStruct*)self->ue_object;
		uint8* data = (uint8*)FMemory::Malloc(u_script_struct->GetStructureSize());
		u_script_struct->InitializeStruct(data);
#if WITH_EDITOR
		u_script_struct->InitializeDefaultValue(data);
#endif
		if (kw)
		{
			PyObject* struct_keys = PyObject_GetIter(kw);
			for (;;)
			{
				PyObject* key = PyIter_Next(struct_keys);
				if (!key)
				{
					if (PyErr_Occurred())
					{
						FMemory::Free(data);
						return PyErr_Format(PyExc_Exception, "unable to build struct from dictionary");
					}
					break;
				}
				if (!PyUnicodeOrString_Check(key))
					continue;
				const char* struct_key = UEPyUnicode_AsUTF8(key);

				PyObject* value = PyDict_GetItem(kw, key);
				if (!value)
				{
					if (PyErr_Occurred())
					{
						FMemory::Free(data);
						return PyErr_Format(PyExc_Exception, "unable to build struct from dictionary");
					}
					break;
				}

				UProperty* u_property = ue_struct_get_field_from_name(u_script_struct, (char*)struct_key);
				if (u_property)
				{
					if (!ue_py_convert_pyobject(value, u_property, data, 0))
					{
						FMemory::Free(data);
						return PyErr_Format(PyExc_Exception, "invalid value for UProperty");
					}
				}
				else
				{
					FMemory::Free(data);
					return PyErr_Format(PyExc_Exception, "UProperty %s not found", struct_key);
				}
			}
		}
		return py_ue_new_owned_uscriptstruct_zero_copy(u_script_struct, data);
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
	"Unreal Engine UObject wrapper",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyUObject_methods,             /* tp_methods */
};






UClass* unreal_engine_new_uclass(char* name, UClass* outer_parent)
{
	bool is_overwriting = false;

	UObject* outer = GetTransientPackage();
	UClass* parent = UObject::StaticClass();

	if (outer_parent)
	{
		parent = outer_parent;
		outer = parent->GetOuter();
	}

	UClass* new_object = FindObject<UClass>(ANY_PACKAGE, UTF8_TO_TCHAR(name));
	if (!new_object)
	{
		new_object = NewObject<UPythonClass>(outer, UTF8_TO_TCHAR(name), RF_Public | RF_Transient | RF_MarkAsNative);
		if (!new_object)
			return nullptr;
	}
	else
	{
		UE_LOG(LogPython, Warning, TEXT("Preparing for overwriting class %s ..."), UTF8_TO_TCHAR(name));
		is_overwriting = true;
	}

	if (is_overwriting && new_object->Children)
	{
		UField* u_field = new_object->Children;
		while (u_field)
		{
			if (u_field->IsA<UFunction>())
			{
				UE_LOG(LogPython, Warning, TEXT("removing function %s"), *u_field->GetName());
				new_object->RemoveFunctionFromFunctionMap((UFunction*)u_field);
				FLinkerLoad::InvalidateExport(u_field);
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

#if WITH_EDITOR
	new_object->SetMetaData(FBlueprintMetadata::MD_AllowableBlueprintVariableType, TEXT("true"));
	if (new_object->IsChildOf<UActorComponent>())
	{
		new_object->SetMetaData(FBlueprintMetadata::MD_BlueprintSpawnableComponent, TEXT("true"));
	}
#endif

	new_object->ClassCastFlags = parent->ClassCastFlags;


	new_object->Bind();
	new_object->StaticLink(true);

	// it could be a class update
	if (is_overwriting && new_object->ClassDefaultObject)
	{
		new_object->GetDefaultObject()->RemoveFromRoot();
		new_object->GetDefaultObject()->ConditionalBeginDestroy();
		new_object->ClassDefaultObject = nullptr;
	}

#if WITH_EDITOR
	new_object->PostEditChange();
#endif

	new_object->GetDefaultObject()->PostInitProperties();

#if WITH_EDITOR
	new_object->PostLinkerChange();
#endif

	new_object->AssembleReferenceTokenStream();

#if WITH_EDITOR
	// this is required for avoiding startup crashes #405
	if (GEditor)
	{
		FBlueprintActionDatabase::Get().RefreshClassActions(new_object);
	}
#endif

	return new_object;
}



int unreal_engine_py_init(ue_PyUObject*, PyObject*, PyObject*);

void unreal_engine_init_py_module()
{
#if PY_MAJOR_VERSION >= 3
	PyObject * new_unreal_engine_module = PyImport_AddModule("unreal_engine");
#else
	PyObject* new_unreal_engine_module = Py_InitModule3("unreal_engine", NULL, unreal_engine_py_doc);
#endif
	ue_PyUObjectType.tp_new = PyType_GenericNew;
	ue_PyUObjectType.tp_init = (initproc)unreal_engine_py_init;
	ue_PyUObjectType.tp_dictoffset = offsetof(ue_PyUObject, py_dict);

	if (PyType_Ready(&ue_PyUObjectType) < 0)
		return;

	Py_INCREF(&ue_PyUObjectType);
	PyModule_AddObject(new_unreal_engine_module, "UObject", (PyObject*)& ue_PyUObjectType);

	PyObject* unreal_engine_dict = PyModule_GetDict(new_unreal_engine_module);

	PyMethodDef* unreal_engine_function;
	for (unreal_engine_function = unreal_engine_methods; unreal_engine_function->ml_name != NULL; unreal_engine_function++)
	{
		PyObject* func = PyCFunction_New(unreal_engine_function, NULL);
		PyDict_SetItemString(unreal_engine_dict, unreal_engine_function->ml_name, func);
		Py_DECREF(func);
	}

	ue_python_init_fvector(new_unreal_engine_module);
	ue_python_init_fvector2d(new_unreal_engine_module);
	ue_python_init_frotator(new_unreal_engine_module);
	ue_python_init_ftransform(new_unreal_engine_module);
	ue_python_init_fhitresult(new_unreal_engine_module);
	ue_python_init_fcolor(new_unreal_engine_module);
	ue_python_init_flinearcolor(new_unreal_engine_module);
	ue_python_init_fquat(new_unreal_engine_module);

#if ENGINE_MINOR_VERSION >= 20
	ue_python_init_fframe_number(new_unreal_engine_module);
#endif

	ue_python_init_frandomstream(new_unreal_engine_module);

	ue_python_init_fraw_anim_sequence_track(new_unreal_engine_module);

#if WITH_EDITOR
	ue_python_init_fsoft_skin_vertex(new_unreal_engine_module);
#endif

	ue_python_init_fmorph_target_delta(new_unreal_engine_module);

	ue_python_init_fobject_thumbnail(new_unreal_engine_module);

	ue_python_init_fviewport_client(new_unreal_engine_module);
#if WITH_EDITOR
	ue_python_init_feditor_viewport_client(new_unreal_engine_module);
	ue_python_init_iasset_editor_instance(new_unreal_engine_module);
#endif

	ue_python_init_fpython_output_device(new_unreal_engine_module);

	ue_python_init_ftimerhandle(new_unreal_engine_module);

	ue_python_init_fdelegatehandle(new_unreal_engine_module);

	ue_python_init_fsocket(new_unreal_engine_module);

	ue_python_init_callable(new_unreal_engine_module);

	ue_python_init_uscriptstruct(new_unreal_engine_module);

	ue_python_init_uclassesimporter(new_unreal_engine_module);
	ue_python_init_enumsimporter(new_unreal_engine_module);
	ue_python_init_ustructsimporter(new_unreal_engine_module);

#if WITH_EDITOR
	ue_python_init_ffoliage_instance(new_unreal_engine_module);

	ue_python_init_fslowtask(new_unreal_engine_module);
	ue_python_init_swidget(new_unreal_engine_module);
	ue_python_init_farfilter(new_unreal_engine_module);
	ue_python_init_fassetdata(new_unreal_engine_module);
	ue_python_init_edgraphpin(new_unreal_engine_module);
	ue_python_init_fstring_asset_reference(new_unreal_engine_module);
#if ENGINE_MINOR_VERSION > 12
	ue_python_init_fbx(new_unreal_engine_module);
#endif
#if ENGINE_MINOR_VERSION > 13
	ue_python_init_fraw_mesh(new_unreal_engine_module);
#endif
	ue_python_init_iplugin(new_unreal_engine_module);
#endif

	ue_python_init_slate(new_unreal_engine_module);


	ue_python_init_ihttp_base(new_unreal_engine_module);
	ue_python_init_ihttp_request(new_unreal_engine_module);
	ue_python_init_ihttp_response(new_unreal_engine_module);

	ue_python_init_iconsole_manager(new_unreal_engine_module);

	ue_python_init_fslate_application(new_unreal_engine_module);

#if WITH_EDITOR
	ue_python_init_fmaterial_editor_utilities(new_unreal_engine_module);
	ue_python_init_icollection_manager(new_unreal_engine_module);
#endif

	ue_python_init_ivoice_capture(new_unreal_engine_module);

	ue_py_register_magic_module((char*)"unreal_engine.classes", py_ue_new_uclassesimporter);
	ue_py_register_magic_module((char*)"unreal_engine.enums", py_ue_new_enumsimporter);
	ue_py_register_magic_module((char*)"unreal_engine.structs", py_ue_new_ustructsimporter);


	PyDict_SetItemString(unreal_engine_dict, "ENGINE_MAJOR_VERSION", PyLong_FromLong(ENGINE_MAJOR_VERSION));
	PyDict_SetItemString(unreal_engine_dict, "ENGINE_MINOR_VERSION", PyLong_FromLong(ENGINE_MINOR_VERSION));
	PyDict_SetItemString(unreal_engine_dict, "ENGINE_PATCH_VERSION", PyLong_FromLong(ENGINE_PATCH_VERSION));

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

	// Classes
	PyDict_SetItemString(unreal_engine_dict, "CLASS_CONFIG", PyLong_FromUnsignedLongLong((uint64)CLASS_Config));
	PyDict_SetItemString(unreal_engine_dict, "CLASS_DEFAULT_CONFIG", PyLong_FromUnsignedLongLong((uint64)CLASS_DefaultConfig));
	PyDict_SetItemString(unreal_engine_dict, "CLASS_ABSTRACT", PyLong_FromUnsignedLongLong((uint64)CLASS_Abstract));
	PyDict_SetItemString(unreal_engine_dict, "CLASS_INTERFACE", PyLong_FromUnsignedLongLong((uint64)CLASS_Interface));

	// Objects
	PyDict_SetItemString(unreal_engine_dict, "RF_NOFLAGS", PyLong_FromUnsignedLongLong((uint64)RF_NoFlags));
	PyDict_SetItemString(unreal_engine_dict, "RF_PUBLIC", PyLong_FromUnsignedLongLong((uint64)RF_Public));
	PyDict_SetItemString(unreal_engine_dict, "RF_STANDALONE", PyLong_FromUnsignedLongLong((uint64)RF_Standalone));
	PyDict_SetItemString(unreal_engine_dict, "RF_MARKASNATIVE", PyLong_FromUnsignedLongLong((uint64)RF_MarkAsNative));
	PyDict_SetItemString(unreal_engine_dict, "RF_TRANSACTIONAL", PyLong_FromUnsignedLongLong((uint64)RF_Transactional));
	PyDict_SetItemString(unreal_engine_dict, "RF_CLASSDEFAULTOBJECT", PyLong_FromUnsignedLongLong((uint64)RF_ClassDefaultObject));
	PyDict_SetItemString(unreal_engine_dict, "RF_CLASS_DEFAULT_OBJECT", PyLong_FromUnsignedLongLong((uint64)RF_ClassDefaultObject));
	PyDict_SetItemString(unreal_engine_dict, "RF_ARCHETYPEOBJECT", PyLong_FromUnsignedLongLong((uint64)RF_ArchetypeObject));
	PyDict_SetItemString(unreal_engine_dict, "RF_ARCHETYPE_OBJECT", PyLong_FromUnsignedLongLong((uint64)RF_ArchetypeObject));
	PyDict_SetItemString(unreal_engine_dict, "RF_TRANSIENT", PyLong_FromUnsignedLongLong((uint64)RF_Transient));
	PyDict_SetItemString(unreal_engine_dict, "RF_MARKASROOTSET", PyLong_FromUnsignedLongLong((uint64)RF_MarkAsRootSet));
	PyDict_SetItemString(unreal_engine_dict, "RF_TAGGARBAGETEMP", PyLong_FromUnsignedLongLong((uint64)RF_TagGarbageTemp));
	PyDict_SetItemString(unreal_engine_dict, "RF_NEEDINITIALIZATION", PyLong_FromUnsignedLongLong((uint64)RF_NeedInitialization));
	PyDict_SetItemString(unreal_engine_dict, "RF_NEEDLOAD", PyLong_FromUnsignedLongLong((uint64)RF_NeedLoad));
	PyDict_SetItemString(unreal_engine_dict, "RF_KEEPFORCOOKER", PyLong_FromUnsignedLongLong((uint64)RF_KeepForCooker));
	PyDict_SetItemString(unreal_engine_dict, "RF_NEEDPOSTLOAD", PyLong_FromUnsignedLongLong((uint64)RF_NeedPostLoad));
	PyDict_SetItemString(unreal_engine_dict, "RF_NEEDPOSTLOADSUBOBJECTS", PyLong_FromUnsignedLongLong((uint64)RF_NeedPostLoadSubobjects));
	PyDict_SetItemString(unreal_engine_dict, "RF_NEWERVERSIONEXISTS", PyLong_FromUnsignedLongLong((uint64)RF_NewerVersionExists));
	PyDict_SetItemString(unreal_engine_dict, "RF_BEGINDESTROYED", PyLong_FromUnsignedLongLong((uint64)RF_BeginDestroyed));
	PyDict_SetItemString(unreal_engine_dict, "RF_FINISHDESTROYED", PyLong_FromUnsignedLongLong((uint64)RF_FinishDestroyed));
	PyDict_SetItemString(unreal_engine_dict, "RF_BEINGREGENERATED", PyLong_FromUnsignedLongLong((uint64)RF_BeingRegenerated));
	PyDict_SetItemString(unreal_engine_dict, "RF_DEFAULTSUBOBJECT", PyLong_FromUnsignedLongLong((uint64)RF_DefaultSubObject));
	PyDict_SetItemString(unreal_engine_dict, "RF_WASLOADED", PyLong_FromUnsignedLongLong((uint64)RF_WasLoaded));
	PyDict_SetItemString(unreal_engine_dict, "RF_TEXTEXPORTTRANSIENT", PyLong_FromUnsignedLongLong((uint64)RF_TextExportTransient));
	PyDict_SetItemString(unreal_engine_dict, "RF_LOADCOMPLETED", PyLong_FromUnsignedLongLong((uint64)RF_LoadCompleted));
	PyDict_SetItemString(unreal_engine_dict, "RF_INHERITABLECOMPONENTTEMPLATE", PyLong_FromUnsignedLongLong((uint64)RF_InheritableComponentTemplate));
	PyDict_SetItemString(unreal_engine_dict, "RF_DUPLICATETRANSIENT", PyLong_FromUnsignedLongLong((uint64)RF_DuplicateTransient));
	PyDict_SetItemString(unreal_engine_dict, "RF_STRONGREFONFRAME", PyLong_FromUnsignedLongLong((uint64)RF_StrongRefOnFrame));
	PyDict_SetItemString(unreal_engine_dict, "RF_NONPIEDUPLICATETRANSIENT", PyLong_FromUnsignedLongLong((uint64)RF_NonPIEDuplicateTransient));
	PyDict_SetItemString(unreal_engine_dict, "RF_DYNAMIC", PyLong_FromUnsignedLongLong((uint64)RF_Dynamic));

#if ENGINE_MINOR_VERSION > 15
	PyDict_SetItemString(unreal_engine_dict, "RF_WILLBELOADED", PyLong_FromUnsignedLongLong((uint64)RF_WillBeLoaded));
#endif

	// Properties
	PyDict_SetItemString(unreal_engine_dict, "CPF_CONFIG", PyLong_FromUnsignedLongLong((uint64)CPF_Config));
	PyDict_SetItemString(unreal_engine_dict, "CPF_GLOBAL_CONFIG", PyLong_FromUnsignedLongLong((uint64)CPF_GlobalConfig));
	PyDict_SetItemString(unreal_engine_dict, "CPF_EXPOSE_ON_SPAWN", PyLong_FromUnsignedLongLong((uint64)CPF_ExposeOnSpawn));
	PyDict_SetItemString(unreal_engine_dict, "CPF_NET", PyLong_FromUnsignedLongLong((uint64)CPF_Net));
	PyDict_SetItemString(unreal_engine_dict, "CPF_REP_NOTIFY", PyLong_FromUnsignedLongLong((uint64)CPF_RepNotify));

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

ue_PyUObject* ue_get_python_uobject(UObject* ue_obj)
{
	if (!ue_obj)
		return nullptr;

	ue_PyUObject* ret = FUnrealEnginePythonHouseKeeper::Get()->GetPyUObject(ue_obj);
	if (!ret)
	{
		if (!ue_obj->IsValidLowLevel() || ue_obj->IsPendingKillOrUnreachable())
			return nullptr;

		ue_PyUObject* ue_py_object = (ue_PyUObject*)PyObject_New(ue_PyUObject, &ue_PyUObjectType);
		if (!ue_py_object)
		{
			return nullptr;
		}
		ue_py_object->ue_object = ue_obj;
		ue_py_object->py_proxy = nullptr;
		ue_py_object->auto_rooted = 0;
		ue_py_object->py_dict = PyDict_New();
		ue_py_object->owned = 0;

		FUnrealEnginePythonHouseKeeper::Get()->RegisterPyUObject(ue_obj, ue_py_object);

#if defined(UEPY_MEMORY_DEBUG)
		UE_LOG(LogPython, Warning, TEXT("CREATED UPyObject at %p for %p %s"), ue_py_object, ue_obj, *ue_obj->GetName());
#endif
		return ue_py_object;
	}
	return ret;

}

ue_PyUObject* ue_get_python_uobject_inc(UObject* ue_obj)
{
	ue_PyUObject* ret = ue_get_python_uobject(ue_obj);
	if (ret)
	{
		Py_INCREF(ret);
	}
	return ret;
}

void unreal_engine_py_log_error()
{
	PyObject* type = NULL;
	PyObject* value = NULL;
	PyObject* traceback = NULL;

	PyErr_Fetch(&type, &value, &traceback);
	PyErr_NormalizeException(&type, &value, &traceback);

	if (!value)
	{
		PyErr_Clear();
		return;
	}

	char* msg = NULL;
#if PY_MAJOR_VERSION >= 3
	PyObject * zero = PyUnicode_AsUTF8String(PyObject_Str(value));
	if (zero)
	{
		msg = PyBytes_AsString(zero);
	}
#else
	msg = PyString_AsString(PyObject_Str(value));
#endif
	if (!msg)
	{
		PyErr_Clear();
		return;
	}

	UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(msg));

	// taken from uWSGI ;)
	if (!traceback)
	{
		PyErr_Clear();
		return;
	}

	PyObject* traceback_module = PyImport_ImportModule("traceback");
	if (!traceback_module)
	{
		PyErr_Clear();
		return;
	}

	PyObject* traceback_dict = PyModule_GetDict(traceback_module);
	PyObject* format_exception = PyDict_GetItemString(traceback_dict, "format_exception");

	if (format_exception)
	{
		PyObject* ret = PyObject_CallFunctionObjArgs(format_exception, type, value, traceback, NULL);
		if (!ret)
		{
			PyErr_Clear();
			return;
		}
		if (PyList_Check(ret))
		{
			for (int i = 0; i < PyList_Size(ret); i++)
			{
				PyObject* item = PyList_GetItem(ret, i);
				if (item)
				{
					UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(PyObject_Str(item))));
				}
			}
		}
		else
		{
			UE_LOG(LogPython, Error, TEXT("%s"), UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(PyObject_Str(ret))));
		}
	}

	PyErr_Clear();
}

// retrieve a UWorld from a generic UObject (if possible)
UWorld* ue_get_uworld(ue_PyUObject* py_obj)
{

	if (py_obj->ue_object->IsA<UWorld>())
	{
		return (UWorld*)py_obj->ue_object;
	}

	if (py_obj->ue_object->IsA<AActor>())
	{
		AActor* actor = (AActor*)py_obj->ue_object;
		return actor->GetWorld();
	}

	if (py_obj->ue_object->IsA<UActorComponent>())
	{
		UActorComponent* component = (UActorComponent*)py_obj->ue_object;
		return component->GetWorld();
	}

	return nullptr;
}

// retrieve actor from component (if possible)
AActor* ue_get_actor(ue_PyUObject* py_obj)
{
	if (py_obj->ue_object->IsA<AActor>())
	{
		return (AActor*)py_obj->ue_object;
	}

	if (py_obj->ue_object->IsA<UActorComponent>())
	{
		UActorComponent* tmp_component = (UActorComponent*)py_obj->ue_object;
		return tmp_component->GetOwner();
	}
	return nullptr;
}

// convert a property to a python object
PyObject* ue_py_convert_property(UProperty* prop, uint8* buffer, int32 index)
{
	if (auto casted_prop = Cast<UBoolProperty>(prop))
	{
		bool value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		if (value)
		{
			Py_RETURN_TRUE;
		}
		Py_RETURN_FALSE;
	}

	if (auto casted_prop = Cast<UIntProperty>(prop))
	{
		int value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyLong_FromLong(value);
	}

	if (auto casted_prop = Cast<UUInt32Property>(prop))
	{
		uint32 value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyLong_FromUnsignedLong(value);
	}

	if (auto casted_prop = Cast<UInt64Property>(prop))
	{
		long long value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyLong_FromLongLong(value);
	}

	if (auto casted_prop = Cast<UUInt64Property>(prop))
	{
		uint64 value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyLong_FromUnsignedLongLong(value);
	}

	if (auto casted_prop = Cast<UFloatProperty>(prop))
	{
		float value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyFloat_FromDouble(value);
	}

	if (auto casted_prop = Cast<UByteProperty>(prop))
	{
		uint8 value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyLong_FromUnsignedLong(value);
	}

#if ENGINE_MINOR_VERSION >= 15
	if (auto casted_prop = Cast<UEnumProperty>(prop))
	{
		void* prop_addr = casted_prop->ContainerPtrToValuePtr<void>(buffer, index);
		uint64 enum_index = casted_prop->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(prop_addr);
		return PyLong_FromUnsignedLong(enum_index);
	}
#endif

	if (auto casted_prop = Cast<UStrProperty>(prop))
	{
		FString value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	if (auto casted_prop = Cast<UTextProperty>(prop))
	{
		FText value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value.ToString()));
	}

	if (auto casted_prop = Cast<UNameProperty>(prop))
	{
		FName value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value.ToString()));
	}

	if (auto casted_prop = Cast<UObjectPropertyBase>(prop))
	{
		auto value = casted_prop->GetObjectPropertyValue_InContainer(buffer, index);
		if (value)
		{
			Py_RETURN_UOBJECT(value);
		}
		Py_RETURN_NONE;
	}

	if (auto casted_prop = Cast<UClassProperty>(prop))
	{
		auto value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		if (value)
		{
			Py_RETURN_UOBJECT(value);
		}
		return PyErr_Format(PyExc_Exception, "invalid UClass type for %s", TCHAR_TO_UTF8(*casted_prop->GetName()));
	}

	// try to manage known struct first
	if (auto casted_prop = Cast<UStructProperty>(prop))
	{
		if (auto casted_struct = Cast<UScriptStruct>(casted_prop->Struct))
		{
			if (casted_struct == TBaseStructure<FVector>::Get())
			{
				FVector vec = *casted_prop->ContainerPtrToValuePtr<FVector>(buffer, index);
				return py_ue_new_fvector(vec);
			}
			if (casted_struct == TBaseStructure<FVector2D>::Get())
			{
				FVector2D vec = *casted_prop->ContainerPtrToValuePtr<FVector2D>(buffer, index);
				return py_ue_new_fvector2d(vec);
			}
			if (casted_struct == TBaseStructure<FRotator>::Get())
			{
				FRotator rot = *casted_prop->ContainerPtrToValuePtr<FRotator>(buffer, index);
				return py_ue_new_frotator(rot);
			}
			if (casted_struct == TBaseStructure<FTransform>::Get())
			{
				FTransform transform = *casted_prop->ContainerPtrToValuePtr<FTransform>(buffer, index);
				return py_ue_new_ftransform(transform);
			}
			if (casted_struct == FHitResult::StaticStruct())
			{
				FHitResult hit = *casted_prop->ContainerPtrToValuePtr<FHitResult>(buffer, index);
				return py_ue_new_fhitresult(hit);
			}
			if (casted_struct == TBaseStructure<FColor>::Get())
			{
				FColor color = *casted_prop->ContainerPtrToValuePtr<FColor>(buffer, index);
				return py_ue_new_fcolor(color);
			}
			if (casted_struct == TBaseStructure<FLinearColor>::Get())
			{
				FLinearColor color = *casted_prop->ContainerPtrToValuePtr<FLinearColor>(buffer, index);
				return py_ue_new_flinearcolor(color);
			}
			return py_ue_new_uscriptstruct(casted_struct, casted_prop->ContainerPtrToValuePtr<uint8>(buffer, index));
		}
		return PyErr_Format(PyExc_TypeError, "unsupported UStruct type");
	}

	if (auto casted_prop = Cast<UWeakObjectProperty>(prop))
	{
		auto value = casted_prop->GetPropertyValue_InContainer(buffer, index);
		UObject* strong_obj = value.Get();
		if (strong_obj)
		{
			Py_RETURN_UOBJECT(strong_obj);
		}
		// nullptr
		Py_RETURN_NONE;
	}

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(prop))
	{
		Py_RETURN_UOBJECT(casted_prop);
	}

	if (auto casted_prop = Cast<UDelegateProperty>(prop))
	{
		Py_RETURN_UOBJECT(casted_prop);
	}

	if (auto casted_prop = Cast<UArrayProperty>(prop))
	{
		FScriptArrayHelper_InContainer array_helper(casted_prop, buffer, index);

		UProperty* array_prop = casted_prop->Inner;

		// check for TArray<uint8>, so we can use bytearray optimization
		if (auto uint8_tarray = Cast<UByteProperty>(array_prop))
		{
			uint8* buf = array_helper.GetRawPtr();
			return PyByteArray_FromStringAndSize((char*)buf, array_helper.Num());
		}

		PyObject* py_list = PyList_New(0);

		for (int i = 0; i < array_helper.Num(); i++)
		{
			PyObject* item = ue_py_convert_property(array_prop, array_helper.GetRawPtr(i), 0);
			if (!item)
			{
				Py_DECREF(py_list);
				return NULL;
			}
			PyList_Append(py_list, item);
			Py_DECREF(item);
		}

		return py_list;
	}

#if ENGINE_MINOR_VERSION >= 15
	if (auto casted_prop = Cast<UMapProperty>(prop))
	{
		FScriptMapHelper_InContainer map_helper(casted_prop, buffer, index);

		PyObject* py_dict = PyDict_New();

		for (int32 i = 0; i < map_helper.Num(); i++)
		{
			if (map_helper.IsValidIndex(i))
			{

				uint8* ptr = map_helper.GetPairPtr(i);

				PyObject* py_key = ue_py_convert_property(map_helper.KeyProp, ptr, 0);
				if (!py_key)
				{
					Py_DECREF(py_dict);
					return NULL;
				}

				PyObject* py_value = ue_py_convert_property(map_helper.ValueProp, ptr, 0);
				if (!py_value)
				{
					Py_DECREF(py_dict);
					return NULL;
				}

				PyDict_SetItem(py_dict, py_key, py_value);
				Py_DECREF(py_key);
				Py_DECREF(py_value);
			}
		}

		return py_dict;
	}
#endif

	return PyErr_Format(PyExc_Exception, "unsupported value type %s for property %s", TCHAR_TO_UTF8(*prop->GetClass()->GetName()), TCHAR_TO_UTF8(*prop->GetName()));
}

// convert a python object to a property
bool ue_py_convert_pyobject(PyObject* py_obj, UProperty* prop, uint8* buffer, int32 index)
{

	if (PyBool_Check(py_obj))
	{
		auto casted_prop = Cast<UBoolProperty>(prop);
		if (!casted_prop)
			return false;
		if (PyObject_IsTrue(py_obj))
		{
			casted_prop->SetPropertyValue_InContainer(buffer, true, index);
		}
		else
		{
			casted_prop->SetPropertyValue_InContainer(buffer, false, index);
		}
		return true;
	}

	if (PyNumber_Check(py_obj))
	{
		if (auto casted_prop = Cast<UIntProperty>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLong(py_long), index);
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UUInt32Property>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsUnsignedLong(py_long), index);
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UInt64Property>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLongLong(py_long), index);
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UUInt64Property>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsUnsignedLongLong(py_long), index);
			Py_DECREF(py_long);
			return true;
		}
		if (auto casted_prop = Cast<UFloatProperty>(prop))
		{
			PyObject* py_float = PyNumber_Float(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyFloat_AsDouble(py_float), index);
			Py_DECREF(py_float);
			return true;
		}
		if (auto casted_prop = Cast<UByteProperty>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsUnsignedLong(py_long), index);
			Py_DECREF(py_long);
			return true;
		}
#if ENGINE_MINOR_VERSION >= 15
		if (auto casted_prop = Cast<UEnumProperty>(prop))
		{
			PyObject* py_long = PyNumber_Long(py_obj);
			void* prop_addr = casted_prop->ContainerPtrToValuePtr<void>(buffer, index);
			casted_prop->GetUnderlyingProperty()->SetIntPropertyValue(prop_addr, (uint64)PyLong_AsUnsignedLong(py_long));
			Py_DECREF(py_long);
			return true;
		}
#endif


		return false;
	}

	if (PyUnicodeOrString_Check(py_obj))
	{
		if (auto casted_prop = Cast<UStrProperty>(prop))
		{
			casted_prop->SetPropertyValue_InContainer(buffer, UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_obj)), index);
			return true;
		}
		if (auto casted_prop = Cast<UNameProperty>(prop))
		{
			casted_prop->SetPropertyValue_InContainer(buffer, UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_obj)), index);
			return true;
		}
		if (auto casted_prop = Cast<UTextProperty>(prop))
		{
			casted_prop->SetPropertyValue_InContainer(buffer, FText::FromString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_obj))), index);
			return true;
		}
		return false;
	}

	if (PyBytes_Check(py_obj))
	{
		if (auto casted_prop = Cast<UArrayProperty>(prop))
		{
			FScriptArrayHelper_InContainer helper(casted_prop, buffer, index);

			if (auto item_casted_prop = Cast<UByteProperty>(casted_prop->Inner))
			{

				Py_ssize_t pybytes_len = PyBytes_Size(py_obj);
				uint8* buf = (uint8*)PyBytes_AsString(py_obj);


				// fix array helper size
				if (helper.Num() < pybytes_len)
				{
					helper.AddValues(pybytes_len - helper.Num());
				}
				else if (helper.Num() > pybytes_len)
				{
					helper.RemoveValues(pybytes_len, helper.Num() - pybytes_len);
				}


				FMemory::Memcpy(helper.GetRawPtr(), buf, pybytes_len);
				return true;
			}
		}

		return false;
	}

	if (PyByteArray_Check(py_obj))
	{
		if (auto casted_prop = Cast<UArrayProperty>(prop))
		{
			FScriptArrayHelper_InContainer helper(casted_prop, buffer, index);

			if (auto item_casted_prop = Cast<UByteProperty>(casted_prop->Inner))
			{

				Py_ssize_t pybytes_len = PyByteArray_Size(py_obj);
				uint8* buf = (uint8*)PyByteArray_AsString(py_obj);


				// fix array helper size
				if (helper.Num() < pybytes_len)
				{
					helper.AddValues(pybytes_len - helper.Num());
				}
				else if (helper.Num() > pybytes_len)
				{
					helper.RemoveValues(pybytes_len, helper.Num() - pybytes_len);
				}


				FMemory::Memcpy(helper.GetRawPtr(), buf, pybytes_len);

				return true;
			}
		}

		return false;
	}

	if (PyList_Check(py_obj))
	{
		if (auto casted_prop = Cast<UArrayProperty>(prop))
		{
			FScriptArrayHelper_InContainer helper(casted_prop, buffer, index);

			UProperty* array_prop = casted_prop->Inner;
			Py_ssize_t pylist_len = PyList_Size(py_obj);

			// fix array helper size
			if (helper.Num() < pylist_len)
			{
				helper.AddValues(pylist_len - helper.Num());
			}
			else if (helper.Num() > pylist_len)
			{
				helper.RemoveValues(pylist_len, helper.Num() - pylist_len);
			}

			for (int i = 0; i < (int)pylist_len; i++)
			{
				PyObject* py_item = PyList_GetItem(py_obj, i);
				if (!ue_py_convert_pyobject(py_item, array_prop, helper.GetRawPtr(i), 0))
				{
					return false;
				}
			}
			return true;
		}

		return false;
	}

	if (PyTuple_Check(py_obj))
	{
		if (auto casted_prop = Cast<UArrayProperty>(prop))
		{
			FScriptArrayHelper_InContainer helper(casted_prop, buffer, index);

			UProperty* array_prop = casted_prop->Inner;
			Py_ssize_t pytuple_len = PyTuple_Size(py_obj);

			// fix array helper size
			if (helper.Num() < pytuple_len)
			{
				helper.AddValues(pytuple_len - helper.Num());
			}
			else if (helper.Num() > pytuple_len)
			{
				helper.RemoveValues(pytuple_len, helper.Num() - pytuple_len);
			}

			for (int i = 0; i < (int)pytuple_len; i++)
			{
				PyObject* py_item = PyTuple_GetItem(py_obj, i);
				if (!ue_py_convert_pyobject(py_item, array_prop, helper.GetRawPtr(i), 0))
				{
					return false;
				}
			}
			return true;
		}

		return false;
	}

#if ENGINE_MINOR_VERSION >= 15
	if (PyDict_Check(py_obj))
	{
		if (auto casted_prop = Cast<UMapProperty>(prop))
		{
			FScriptMapHelper_InContainer map_helper(casted_prop, buffer, index);

			PyObject* py_key = nullptr;
			PyObject* py_value = nullptr;
			Py_ssize_t pos = 0;

			map_helper.EmptyValues();
			while (PyDict_Next(py_obj, &pos, &py_key, &py_value))
			{

				int32 hindex = map_helper.AddDefaultValue_Invalid_NeedsRehash();
				uint8* ptr = map_helper.GetPairPtr(hindex);

				if (!ue_py_convert_pyobject(py_key, casted_prop->KeyProp, ptr, 0))
				{
					return false;
				}

				if (!ue_py_convert_pyobject(py_value, casted_prop->ValueProp, ptr, 0))
				{
					return false;
				}
			}
			map_helper.Rehash();

			return true;
		}

		return false;
	}
#endif

	// structs

	if (ue_PyFVector * py_vec = py_ue_is_fvector(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FVector>::Get())
			{
				*casted_prop->ContainerPtrToValuePtr<FVector>(buffer, index) = py_vec->vec;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFVector2D * py_vec = py_ue_is_fvector2d(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FVector2D>::Get())
			{
				*casted_prop->ContainerPtrToValuePtr<FVector2D>(buffer, index) = py_vec->vec;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFRotator * py_rot = py_ue_is_frotator(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FRotator>::Get())
			{
				*casted_prop->ContainerPtrToValuePtr<FRotator>(buffer, index) = py_rot->rot;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFTransform * py_transform = py_ue_is_ftransform(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FTransform>::Get())
			{
				*casted_prop->ContainerPtrToValuePtr<FTransform>(buffer, index) = py_transform->transform;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFColor * py_color = py_ue_is_fcolor(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FColor>::Get())
			{

				*casted_prop->ContainerPtrToValuePtr<FColor>(buffer, index) = py_color->color;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFLinearColor * py_color = py_ue_is_flinearcolor(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == TBaseStructure<FLinearColor>::Get())
			{
				*casted_prop->ContainerPtrToValuePtr<FLinearColor>(buffer, index) = py_color->color;
				return true;
			}
		}
		return false;
	}

	if (ue_PyFHitResult * py_hit = py_ue_is_fhitresult(py_obj))
	{
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == FHitResult::StaticStruct())
			{
				*casted_prop->ContainerPtrToValuePtr<FHitResult>(buffer, index) = py_hit->hit;
				return true;
			}
		}
		return false;
	}

	// generic structs
	if (py_ue_is_uscriptstruct(py_obj))
	{
		ue_PyUScriptStruct* py_u_struct = (ue_PyUScriptStruct*)py_obj;
		if (auto casted_prop = Cast<UStructProperty>(prop))
		{
			if (casted_prop->Struct == py_u_struct->u_struct)
			{
				uint8* dest = casted_prop->ContainerPtrToValuePtr<uint8>(buffer, index);
				py_u_struct->u_struct->InitializeStruct(dest);
				py_u_struct->u_struct->CopyScriptStruct(dest, py_u_struct->u_struct_ptr);
				return true;
			}
		}
		return false;
	}

	if (PyObject_IsInstance(py_obj, (PyObject*)& ue_PyUObjectType))
	{
		ue_PyUObject* ue_obj = (ue_PyUObject*)py_obj;
		if (ue_obj->ue_object->IsA<UClass>())
		{
			if (auto casted_prop = Cast<UClassProperty>(prop))
			{
				casted_prop->SetPropertyValue_InContainer(buffer, ue_obj->ue_object, index);
				return true;
			}
			else if (auto casted_prop_soft_class = Cast<USoftClassProperty>(prop))
			{
				casted_prop_soft_class->SetPropertyValue_InContainer(buffer, FSoftObjectPtr(ue_obj->ue_object), index);
				return true;
			}
			else if (auto casted_prop_soft_object = Cast<USoftObjectProperty>(prop))
			{

				casted_prop_soft_object->SetPropertyValue_InContainer(buffer, FSoftObjectPtr(ue_obj->ue_object), index);

				return true;
			}
			else if (auto casted_prop_weak_object = Cast<UWeakObjectProperty>(prop))
			{

				casted_prop_weak_object->SetPropertyValue_InContainer(buffer, FWeakObjectPtr(ue_obj->ue_object), index);

				return true;
			}
			else if (auto casted_prop_base = Cast<UObjectPropertyBase>(prop))
			{
				// ensure the object type is correct, otherwise crash could happen (soon or later)
				if (!ue_obj->ue_object->IsA(casted_prop_base->PropertyClass))
					return false;

				casted_prop_base->SetObjectPropertyValue_InContainer(buffer, ue_obj->ue_object, index);

				return true;
			}

			return false;
		}


		if (ue_obj->ue_object->IsA<UObject>())
		{
			if (auto casted_prop = Cast<UObjectPropertyBase>(prop))
			{
				// if the property specifies an interface, the object must be of a class that implements it
				if (casted_prop->PropertyClass->HasAnyClassFlags(CLASS_Interface))
				{
					if (!ue_obj->ue_object->GetClass()->ImplementsInterface(casted_prop->PropertyClass))
						return false;
				}
				else
				{
					// ensure the object type is correct, otherwise crash could happen (soon or later)
					if (!ue_obj->ue_object->IsA(casted_prop->PropertyClass))
						return false;
				}

				casted_prop->SetObjectPropertyValue_InContainer(buffer, ue_obj->ue_object, index);

				return true;
			}
			else if (auto casted_prop_soft_object = Cast<USoftObjectProperty>(prop))
			{
				if (!ue_obj->ue_object->IsA(casted_prop_soft_object->PropertyClass))
					return false;

				casted_prop_soft_object->SetPropertyValue_InContainer(buffer, FSoftObjectPtr(ue_obj->ue_object), index);

				return true;
			}
			else if (auto casted_prop_interface = Cast<UInterfaceProperty>(prop))
			{
				// ensure the object type is correct, otherwise crash could happen (soon or later)
				if (!ue_obj->ue_object->GetClass()->ImplementsInterface(casted_prop_interface->InterfaceClass))
					return false;

				casted_prop_interface->SetPropertyValue_InContainer(buffer, FScriptInterface(ue_obj->ue_object), index);

				return true;
			}
		}
		return false;
	}

	if (py_obj == Py_None)
	{
		auto casted_prop_class = Cast<UClassProperty>(prop);
		if (casted_prop_class)
		{

			casted_prop_class->SetPropertyValue_InContainer(buffer, nullptr, index);

			return true;
		}
		auto casted_prop = Cast<UObjectPropertyBase>(prop);
		if (casted_prop)
		{

			casted_prop->SetObjectPropertyValue_InContainer(buffer, nullptr, index);

			return true;
		}
		return false;
	}

	return false;

}


// check if a python object is a wrapper to a UObject
ue_PyUObject* ue_is_pyuobject(PyObject* obj)
{
	if (!PyObject_IsInstance(obj, (PyObject*)& ue_PyUObjectType))
		return nullptr;
	return (ue_PyUObject*)obj;
}

void ue_bind_events_for_py_class_by_attribute(UObject* u_obj, PyObject* py_class)
{
	// attempt to register events
	PyObject* attrs = PyObject_Dir(py_class);
	if (!attrs)
		return;

	AActor* actor = Cast<AActor>(u_obj);
	if (!actor)
	{
		UActorComponent* component = Cast<UActorComponent>(u_obj);
		if (!component)
			return;
		actor = component->GetOwner();
	}

	Py_ssize_t len = PyList_Size(attrs);
	for (Py_ssize_t i = 0; i < len; i++)
	{
		PyObject* py_attr_name = PyList_GetItem(attrs, i);
		if (!py_attr_name || !PyUnicodeOrString_Check(py_attr_name))
			continue;
		PyObject* item = PyObject_GetAttrString(py_class, UEPyUnicode_AsUTF8(py_attr_name));
		if (item && PyCallable_Check(item))
		{
			// check for ue_event signature
			PyObject* event_signature = PyObject_GetAttrString(item, (char*)"ue_event");
			if (event_signature)
			{
				if (PyUnicodeOrString_Check(event_signature))
				{
					FString event_name = FString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(event_signature)));
					TArray<FString> parts;
					int n = event_name.ParseIntoArray(parts, UTF8_TO_TCHAR("."));
					if (n < 1 || n > 2)
					{
						PyErr_SetString(PyExc_Exception, "invalid ue_event syntax, must be the name of an event or ComponentName.Event");
						unreal_engine_py_log_error();
					}
					else
					{
						if (n == 1)
						{
							if (!ue_bind_pyevent(ue_get_python_uobject(actor), parts[0], item, true))
							{
								unreal_engine_py_log_error();
							}
						}
						else
						{
							bool found = false;
							for (UActorComponent* component : actor->GetComponents())
							{
								if (component->GetFName() == FName(*parts[0]))
								{
									if (!ue_bind_pyevent(ue_get_python_uobject(component), parts[1], item, true))
									{
										unreal_engine_py_log_error();
									}
									found = true;
									break;
								}
							}

							if (!found)
							{
								PyErr_SetString(PyExc_Exception, "unable to find component by name");
								unreal_engine_py_log_error();
							}
						}
					}
				}
				else
				{
					PyErr_SetString(PyExc_Exception, "ue_event attribute must be a string");
					unreal_engine_py_log_error();
				}
			}
			Py_XDECREF(event_signature);
		}
		Py_XDECREF(item);
	}
	Py_DECREF(attrs);

	PyErr_Clear();
}

// automatically bind events based on class methods names
void ue_autobind_events_for_pyclass(ue_PyUObject* u_obj, PyObject* py_class)
{
	PyObject* attrs = PyObject_Dir(py_class);
	if (!attrs)
		return;

	Py_ssize_t len = PyList_Size(attrs);
	for (Py_ssize_t i = 0; i < len; i++)
	{
		PyObject* py_attr_name = PyList_GetItem(attrs, i);
		if (!py_attr_name || !PyUnicodeOrString_Check(py_attr_name))
			continue;
		FString attr_name = UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(py_attr_name));
		if (!attr_name.StartsWith("on_", ESearchCase::CaseSensitive))
			continue;
		// check if the attr is a callable
		PyObject* item = PyObject_GetAttrString(py_class, TCHAR_TO_UTF8(*attr_name));
		if (item && PyCallable_Check(item))
		{
			TArray<FString> parts;
			if (attr_name.ParseIntoArray(parts, UTF8_TO_TCHAR("_")) > 1)
			{
				FString event_name;
				for (FString part : parts)
				{
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

static void py_ue_destroy_params(UFunction* u_function, uint8* buffer)
{
	// destroy params
	TFieldIterator<UProperty> DArgs(u_function);
	for (; DArgs && (DArgs->PropertyFlags & CPF_Parm); ++DArgs)
	{
		UProperty* prop = *DArgs;
		prop->DestroyValue_InContainer(buffer);
	}
}

PyObject* py_ue_ufunction_call(UFunction* u_function, UObject* u_obj, PyObject* args, int argn, PyObject* kwargs)
{

	// check for __super call
	if (kwargs)
	{
		PyObject* is_super_call = PyDict_GetItemString(kwargs, (char*)"__super");
		if (is_super_call)
		{
			if (!u_function->GetSuperFunction())
			{
				return PyErr_Format(PyExc_Exception, "UFunction has no SuperFunction");
			}
			u_function = u_function->GetSuperFunction();
		}
	}

	//NOTE: u_function->PropertiesSize maps to local variable uproperties + ufunction paramaters uproperties
	uint8* buffer = (uint8*)FMemory_Alloca(u_function->ParmsSize);
	FMemory::Memzero(buffer, u_function->ParmsSize);
	// initialize args
	for (TFieldIterator<UProperty> IArgs(u_function); IArgs && IArgs->HasAnyPropertyFlags(CPF_Parm); ++IArgs)
	{
		UProperty* prop = *IArgs;
		if (!prop->HasAnyPropertyFlags(CPF_ZeroConstructor))
		{
			prop->InitializeValue_InContainer(buffer);
		}

		//UObject::CallFunctionByNameWithArguments() only does this part on non return value params
		if ((IArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm)
		{
			if (!prop->IsInContainer(u_function->ParmsSize))
			{
				return PyErr_Format(PyExc_Exception, "Attempting to import func param property that's out of bounds. %s", TCHAR_TO_UTF8(*u_function->GetName()));
			}
#if WITH_EDITOR
			FString default_key = FString("CPP_Default_") + prop->GetName();
			FString default_key_value = u_function->GetMetaData(FName(*default_key));
			if (!default_key_value.IsEmpty())
			{
#if ENGINE_MINOR_VERSION >= 17
				prop->ImportText(*default_key_value, prop->ContainerPtrToValuePtr<uint8>(buffer), PPF_None, NULL);
#else
				prop->ImportText(*default_key_value, prop->ContainerPtrToValuePtr<uint8>(buffer), PPF_Localized, NULL);
#endif
			}
#endif
		}
	}


	Py_ssize_t tuple_len = PyTuple_Size(args);

	int has_out_params = 0;

	TFieldIterator<UProperty> PArgs(u_function);
	for (; PArgs && ((PArgs->PropertyFlags & (CPF_Parm | CPF_ReturnParm)) == CPF_Parm); ++PArgs)
	{
		UProperty* prop = *PArgs;
		if (argn < tuple_len)
		{
			PyObject* py_arg = PyTuple_GetItem(args, argn);
			if (!py_arg)
			{
				py_ue_destroy_params(u_function, buffer);
				return PyErr_Format(PyExc_TypeError, "unable to get pyobject for property %s", TCHAR_TO_UTF8(*prop->GetName()));
			}
			if (!ue_py_convert_pyobject(py_arg, prop, buffer, 0))
			{
				py_ue_destroy_params(u_function, buffer);
				return PyErr_Format(PyExc_TypeError, "unable to convert pyobject to property %s (%s)", TCHAR_TO_UTF8(*prop->GetName()), TCHAR_TO_UTF8(*prop->GetClass()->GetName()));
			}
		}
		else if (kwargs)
		{
			char* prop_name = TCHAR_TO_UTF8(*prop->GetName());
			PyObject* dict_value = PyDict_GetItemString(kwargs, prop_name);
			if (dict_value)
			{
				if (!ue_py_convert_pyobject(dict_value, prop, buffer, 0))
				{
					py_ue_destroy_params(u_function, buffer);
					return PyErr_Format(PyExc_TypeError, "unable to convert pyobject to property %s (%s)", TCHAR_TO_UTF8(*prop->GetName()), TCHAR_TO_UTF8(*prop->GetClass()->GetName()));
				}
			}
		}
		if (prop->HasAnyPropertyFlags(CPF_OutParm) && (prop->IsA<UArrayProperty>() || prop->HasAnyPropertyFlags(CPF_ConstParm) == false))
		{
			has_out_params++;
		}
		argn++;
	}

	FScopeCycleCounterUObject ObjectScope(u_obj);
	FScopeCycleCounterUObject FunctionScope(u_function);

	Py_BEGIN_ALLOW_THREADS;
	u_obj->ProcessEvent(u_function, buffer);
	Py_END_ALLOW_THREADS;

	PyObject* ret = nullptr;

	int has_ret_param = 0;
	TFieldIterator<UProperty> Props(u_function);
	for (; Props; ++Props)
	{
		UProperty* prop = *Props;
		if (prop->GetPropertyFlags() & CPF_ReturnParm)
		{
			ret = ue_py_convert_property(prop, buffer, 0);
			if (!ret)
			{
				// destroy params
				py_ue_destroy_params(u_function, buffer);
				return NULL;
			}
			has_ret_param = 1;
			break;
		}
	}

	if (has_out_params > 0)
	{
		PyObject* multi_ret = PyTuple_New(has_out_params + has_ret_param);
		if (ret)
		{
			PyTuple_SetItem(multi_ret, 0, ret);
		}
		TFieldIterator<UProperty> OProps(u_function);
		for (; OProps; ++OProps)
		{
			UProperty* prop = *OProps;
			if (prop->HasAnyPropertyFlags(CPF_OutParm) && (prop->IsA<UArrayProperty>() || prop->HasAnyPropertyFlags(CPF_ConstParm) == false))
			{
				// skip return param as it must be always the first
				if (prop->GetPropertyFlags() & CPF_ReturnParm)
					continue;
				PyObject* py_out = ue_py_convert_property(prop, buffer, 0);
				if (!py_out)
				{
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

	Py_RETURN_NONE;
}

PyObject* ue_unbind_pyevent(ue_PyUObject* u_obj, FString event_name, PyObject* py_callable, bool fail_on_wrong_property)
{
	UProperty* u_property = u_obj->ue_object->GetClass()->FindPropertyByName(FName(*event_name));
	if (!u_property)
	{
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "unable to find event property %s", TCHAR_TO_UTF8(*event_name));
		Py_RETURN_NONE;
	}

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(u_property))
	{
		UPythonDelegate* py_delegate = FUnrealEnginePythonHouseKeeper::Get()->FindDelegate(u_obj->ue_object, py_callable);
		if (py_delegate != nullptr)
		{
#if ENGINE_MINOR_VERSION < 23
			FMulticastScriptDelegate multiscript_delegate = casted_prop->GetPropertyValue_InContainer(u_obj->ue_object);
#else
			FMulticastScriptDelegate multiscript_delegate = *casted_prop->GetMulticastDelegate(u_obj->ue_object);
#endif

			multiscript_delegate.Remove(py_delegate, FName("PyFakeCallable"));

			// re-assign multicast delegate
#if ENGINE_MINOR_VERSION < 23
			casted_prop->SetPropertyValue_InContainer(u_obj->ue_object, multiscript_delegate);
#else
			casted_prop->SetMulticastDelegate(u_obj->ue_object, multiscript_delegate);
#endif
		}
	}
	else if (auto casted_prop_delegate = Cast<UDelegateProperty>(u_property))
	{
		FScriptDelegate script_delegate = casted_prop_delegate->GetPropertyValue_InContainer(u_obj->ue_object);
		script_delegate.Unbind();

		// re-assign multicast delegate
		casted_prop_delegate->SetPropertyValue_InContainer(u_obj->ue_object, script_delegate);
	}
	else
	{
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "property %s is not an event", TCHAR_TO_UTF8(*event_name));
	}

	Py_RETURN_NONE;
}

PyObject* ue_bind_pyevent(ue_PyUObject* u_obj, FString event_name, PyObject* py_callable, bool fail_on_wrong_property)
{

	UProperty* u_property = u_obj->ue_object->GetClass()->FindPropertyByName(FName(*event_name));
	if (!u_property)
	{
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "unable to find event property %s", TCHAR_TO_UTF8(*event_name));
		Py_RETURN_NONE;
	}

	if (auto casted_prop = Cast<UMulticastDelegateProperty>(u_property))
	{
#if ENGINE_MINOR_VERSION < 23
		FMulticastScriptDelegate multiscript_delegate = casted_prop->GetPropertyValue_InContainer(u_obj->ue_object);
#else
		FMulticastScriptDelegate multiscript_delegate = *casted_prop->GetMulticastDelegate(u_obj->ue_object);
#endif

		FScriptDelegate script_delegate;
		UPythonDelegate* py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDelegate(u_obj->ue_object, py_callable, casted_prop->SignatureFunction);
		// fake UFUNCTION for bypassing checks
		script_delegate.BindUFunction(py_delegate, FName("PyFakeCallable"));

		// add the new delegate
		multiscript_delegate.Add(script_delegate);

		// re-assign multicast delegate
#if ENGINE_MINOR_VERSION < 23
		casted_prop->SetPropertyValue_InContainer(u_obj->ue_object, multiscript_delegate);
#else
		casted_prop->SetMulticastDelegate(u_obj->ue_object, multiscript_delegate);
#endif
	}
	else if (auto casted_prop_delegate = Cast<UDelegateProperty>(u_property))
	{

		FScriptDelegate script_delegate = casted_prop_delegate->GetPropertyValue_InContainer(u_obj->ue_object);
		UPythonDelegate* py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDelegate(u_obj->ue_object, py_callable, casted_prop_delegate->SignatureFunction);
		// fake UFUNCTION for bypassing checks
		script_delegate.BindUFunction(py_delegate, FName("PyFakeCallable"));

		// re-assign multicast delegate
		casted_prop_delegate->SetPropertyValue_InContainer(u_obj->ue_object, script_delegate);
	}
	else
	{
		if (fail_on_wrong_property)
			return PyErr_Format(PyExc_Exception, "property %s is not an event", TCHAR_TO_UTF8(*event_name));
	}

	Py_RETURN_NONE;
}

UFunction* unreal_engine_add_function(UClass* u_class, char* name, PyObject* py_callable, uint32 function_flags)
{

	UFunction* parent_function = u_class->GetSuperClass()->FindFunctionByName(UTF8_TO_TCHAR(name));
	// if the function is not available in the parent
	// check for name collision
	if (!parent_function)
	{
		if (u_class->FindFunctionByName(UTF8_TO_TCHAR(name)))
		{
			UE_LOG(LogPython, Error, TEXT("function %s is already registered"), UTF8_TO_TCHAR(name));
			return nullptr;
		}
	}

	UPythonFunction* function = NewObject<UPythonFunction>(u_class, UTF8_TO_TCHAR(name), RF_Public | RF_Transient | RF_MarkAsNative);
	function->SetPyCallable(py_callable);

#if ENGINE_MINOR_VERSION < 18
	function->RepOffset = MAX_uint16;
#endif
	function->ReturnValueOffset = MAX_uint16;
	function->FirstPropertyToInit = NULL;
	function->Script.Add(EX_EndFunctionParms);

	if (parent_function)
	{
		function->SetSuperStruct(parent_function);
		function_flags |= (parent_function->FunctionFlags & FUNC_FuncInherit);
	}

	// iterate all arguments using inspect.signature()
	// this is required to maintaining args order
	PyObject* inspect = PyImport_ImportModule("inspect");
	if (!inspect)
	{
		return NULL;
	}
	PyObject* signature = PyObject_CallMethod(inspect, (char*)"signature", (char*)"O", py_callable);
	if (!signature)
	{
		return NULL;
	}

	PyObject* parameters = PyObject_GetAttrString(signature, "parameters");
	if (!parameters)
	{
		return NULL;
	}

	PyObject* annotations = PyObject_GetAttrString(py_callable, "__annotations__");

	UField** next_property = &function->Children;
	UProperty** next_property_link = &function->PropertyLink;

	PyObject* parameters_keys = PyObject_GetIter(parameters);
	// do not process args if no annotations are available

	while (annotations)
	{
		PyObject* key = PyIter_Next(parameters_keys);
		if (!key)
		{
			if (PyErr_Occurred())
				return NULL;
			break;
		}
		if (!PyUnicodeOrString_Check(key))
			continue;

		const char* p_name = UEPyUnicode_AsUTF8(key);

		PyObject* value = PyDict_GetItem(annotations, key);
		if (!value)
			continue;

		UProperty* prop = nullptr;
		if (PyType_Check(value))
		{
			if ((PyTypeObject*)value == &PyFloat_Type)
			{
				prop = NewObject<UFloatProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject*)value == &PyUnicode_Type)
			{
				prop = NewObject<UStrProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject*)value == &PyBool_Type)
			{
				prop = NewObject<UBoolProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject*)value == &PyLong_Type)
			{
				prop = NewObject<UIntProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
			}
			else if ((PyTypeObject*)value == &ue_PyFVectorType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FVector>::Get();
				prop = prop_struct;
			}
			else if ((PyTypeObject*)value == &ue_PyFVector2DType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FVector2D>::Get();
				prop = prop_struct;
			}
			else if ((PyTypeObject*)value == &ue_PyFRotatorType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FRotator>::Get();
				prop = prop_struct;
			}
			else if ((PyTypeObject*)value == &ue_PyFLinearColorType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FLinearColor>::Get();
				prop = prop_struct;
			}
			else if ((PyTypeObject*)value == &ue_PyFColorType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FColor>::Get();
				prop = prop_struct;
			}
			else if ((PyTypeObject*)value == &ue_PyFTransformType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FTransform>::Get();
				prop = prop_struct;
			}
#if ENGINE_MINOR_VERSION > 18
			else if ((PyTypeObject*)value == &ue_PyFQuatType)
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = TBaseStructure<FQuat>::Get();
				prop = prop_struct;
			}
#endif
			else if (PyObject_IsInstance(value, (PyObject*)& PyType_Type))
			{
				// Method annotation like foo:typing.Type[Pawn] produces annotations like typing.Type[Pawn], with .__args__ = (Pawn,)
				PyObject* type_args = PyObject_GetAttrString(value, "__args__");
				if (!type_args)
				{
					UE_LOG(LogPython, Error, TEXT("missing type info on %s"), UTF8_TO_TCHAR(name));
					return nullptr;
				}
				if (PyTuple_Size(type_args) != 1)
				{
					Py_DECREF(type_args);
					UE_LOG(LogPython, Error, TEXT("exactly one class is allowed in type info for %s"), UTF8_TO_TCHAR(name));
					return nullptr;
				}
				PyObject* py_class = PyTuple_GetItem(type_args, 0);
				ue_PyUObject* py_obj = ue_is_pyuobject(py_class);
				if (!py_obj)
				{
					Py_DECREF(type_args);
					UE_LOG(LogPython, Error, TEXT("type for %s must be a ue_PyUObject"), UTF8_TO_TCHAR(name));
					return nullptr;
				}
				if (!py_obj->ue_object->IsA<UClass>())
				{
					Py_DECREF(type_args);
					UE_LOG(LogPython, Error, TEXT("type for %s must be a UClass"), UTF8_TO_TCHAR(name));
					return nullptr;
				}
				UClassProperty* prop_class = NewObject<UClassProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_class->SetMetaClass((UClass*)py_obj->ue_object);
				prop_class->PropertyClass = UClass::StaticClass();
				prop = prop_class;
				Py_DECREF(type_args);
			}
		}
		else if (ue_PyUObject * py_obj = ue_is_pyuobject(value))
		{
			if (py_obj->ue_object->IsA<UClass>())
			{
				UClass* p_u_class = (UClass*)py_obj->ue_object;
				UObjectProperty* prop_base = NewObject<UObjectProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_base->SetPropertyClass(p_u_class);
				prop = prop_base;
			}
#if ENGINE_MINOR_VERSION > 17
			else if (py_obj->ue_object->IsA<UEnum>())
			{
				UEnumProperty* prop_enum = NewObject<UEnumProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				UNumericProperty* prop_underlying = NewObject<UByteProperty>(prop_enum, TEXT("UnderlyingType"), RF_Public);
				prop_enum->SetEnum((UEnum*)py_obj->ue_object);
				prop_enum->AddCppProperty(prop_underlying);
				prop = prop_enum;
			}
#endif
			else if (py_obj->ue_object->IsA<UStruct>())
			{
				UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				prop_struct->Struct = (UScriptStruct*)py_obj->ue_object;
				prop = prop_struct;
			}
		}

		if (prop)
		{
			prop->SetPropertyFlags(CPF_Parm);
			*next_property = prop;
			next_property = &prop->Next;
			*next_property_link = prop;
			next_property_link = &prop->PropertyLinkNext;
			UE_LOG(LogPython, Warning, TEXT("added prop %s"), UTF8_TO_TCHAR(p_name));
		}
		else
		{
			UE_LOG(LogPython, Warning, TEXT("Unable to map argument %s to function %s"), UTF8_TO_TCHAR(p_name), UTF8_TO_TCHAR(name));
		}
	}

	// check for return value
	if (annotations)
	{
		PyObject* py_return_value = PyDict_GetItemString(annotations, "return");
		if (py_return_value)
		{
			UE_LOG(LogPython, Warning, TEXT("Return Value found"));
			UProperty* prop = nullptr;
			char* p_name = (char*) "ReturnValue";
			if (PyType_Check(py_return_value))
			{
				if ((PyTypeObject*)py_return_value == &PyFloat_Type)
				{
					prop = NewObject<UFloatProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject*)py_return_value == &PyUnicode_Type)
				{
					prop = NewObject<UStrProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject*)py_return_value == &PyBool_Type)
				{
					prop = NewObject<UBoolProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject*)py_return_value == &PyLong_Type)
				{
					prop = NewObject<UIntProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFVectorType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FVector>::Get();
					prop = prop_struct;
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFVector2DType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FVector2D>::Get();
					prop = prop_struct;
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFRotatorType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FRotator>::Get();
					prop = prop_struct;
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFLinearColorType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FLinearColor>::Get();
					prop = prop_struct;
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFColorType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FColor>::Get();
					prop = prop_struct;
				}
				else if ((PyTypeObject*)py_return_value == &ue_PyFTransformType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FTransform>::Get();
					prop = prop_struct;
				}
#if ENGINE_MINOR_VERSION > 18
				else if ((PyTypeObject*)py_return_value == &ue_PyFQuatType)
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = TBaseStructure<FQuat>::Get();
					prop = prop_struct;
				}
#endif
				else if (PyObject_IsInstance(py_return_value, (PyObject*)& PyType_Type))
				{
					// Method annotation like foo:typing.Type[Pawn] produces annotations like typing.Type[Pawn], with .__args__ = (Pawn,)
					PyObject* type_args = PyObject_GetAttrString(py_return_value, "__args__");
					if (!type_args)
					{
						UE_LOG(LogPython, Error, TEXT("missing type info on %s"), UTF8_TO_TCHAR(name));
						return nullptr;
					}
					if (PyTuple_Size(type_args) != 1)
					{
						Py_DECREF(type_args);
						UE_LOG(LogPython, Error, TEXT("exactly one class is allowed in type info for %s"), UTF8_TO_TCHAR(name));
						return nullptr;
					}
					PyObject* py_class = PyTuple_GetItem(type_args, 0);
					ue_PyUObject* py_obj = ue_is_pyuobject(py_class);
					if (!py_obj)
					{
						Py_DECREF(type_args);
						UE_LOG(LogPython, Error, TEXT("type for %s must be a ue_PyUObject"), UTF8_TO_TCHAR(name));
						return nullptr;
					}
					if (!py_obj->ue_object->IsA<UClass>())
					{
						Py_DECREF(type_args);
						UE_LOG(LogPython, Error, TEXT("type for %s must be a UClass"), UTF8_TO_TCHAR(name));
						return nullptr;
					}
					UClassProperty* prop_class = NewObject<UClassProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_class->SetMetaClass((UClass*)py_obj->ue_object);
					prop_class->PropertyClass = UClass::StaticClass();
					prop = prop_class;
					Py_DECREF(type_args);
				}
			}
			else if (ue_PyUObject * py_obj = ue_is_pyuobject(py_return_value))
			{
				if (py_obj->ue_object->IsA<UClass>())
				{
					UClass* p_u_class = (UClass*)py_obj->ue_object;
					UObjectProperty* prop_base = NewObject<UObjectProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_base->SetPropertyClass(p_u_class);
					prop = prop_base;
				}
#if ENGINE_MINOR_VERSION > 17
				else if (py_obj->ue_object->IsA<UEnum>())
				{
					UEnumProperty* prop_enum = NewObject<UEnumProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					UNumericProperty* prop_underlying = NewObject<UByteProperty>(prop_enum, TEXT("UnderlyingType"), RF_Public);
					prop_enum->SetEnum((UEnum*)py_obj->ue_object);
					prop_enum->AddCppProperty(prop_underlying);
					prop = prop_enum;
				}
#endif
				else if (py_obj->ue_object->IsA<UStruct>())
				{
					UStructProperty* prop_struct = NewObject<UStructProperty>(function, UTF8_TO_TCHAR(p_name), RF_Public);
					prop_struct->Struct = (UScriptStruct*)py_obj->ue_object;
					prop = prop_struct;
				}
			}

			if (prop)
			{
				prop->SetPropertyFlags(CPF_Parm | CPF_OutParm | CPF_ReturnParm);
				*next_property = prop;
				next_property = &prop->Next;
				*next_property_link = prop;
				next_property_link = &prop->PropertyLinkNext;
			}
			else
			{
				UE_LOG(LogPython, Warning, TEXT("Unable to map return value to function %s"), UTF8_TO_TCHAR(name));
			}
		}
	}

	// link to fix props Offset_Internal
	function->Bind();
	function->StaticLink(true);

	if (parent_function)
	{

		if (!function->IsSignatureCompatibleWith(parent_function))
		{
			TFieldIterator<UProperty> It(parent_function);
			while (It)
			{
				UProperty* p = *It;
				if (p->PropertyFlags & CPF_Parm)
				{
					UE_LOG(LogPython, Warning, TEXT("Parent PROP: %s %d/%d %d %d %d %s %p"), *p->GetName(), (int)p->PropertyFlags, (int)UFunction::GetDefaultIgnoredSignatureCompatibilityFlags(), (int)(p->PropertyFlags & ~UFunction::GetDefaultIgnoredSignatureCompatibilityFlags()), p->GetSize(), p->GetOffset_ForGC(), *p->GetClass()->GetName(), p->GetClass());
					UClassProperty* ucp = Cast<UClassProperty>(p);
					if (ucp)
					{
						UE_LOG(LogPython, Warning, TEXT("Parent UClassProperty = %p %s %p %s"), ucp->PropertyClass, *ucp->PropertyClass->GetName(), ucp->MetaClass, *ucp->MetaClass->GetName());
					}
				}
				++It;
			}

			TFieldIterator<UProperty> It2(function);
			while (It2)
			{
				UProperty* p = *It2;
				if (p->PropertyFlags & CPF_Parm)
				{
					UE_LOG(LogPython, Warning, TEXT("Function PROP: %s %d/%d %d %d %d %s %p"), *p->GetName(), (int)p->PropertyFlags, (int)UFunction::GetDefaultIgnoredSignatureCompatibilityFlags(), (int)(p->PropertyFlags & ~UFunction::GetDefaultIgnoredSignatureCompatibilityFlags()), p->GetSize(), p->GetOffset_ForGC(), *p->GetClass()->GetName(), p->GetClass());
					UClassProperty* ucp = Cast<UClassProperty>(p);
					if (ucp)
					{
						UE_LOG(LogPython, Warning, TEXT("Function UClassProperty = %p %s %p %s"), ucp->PropertyClass, *ucp->PropertyClass->GetName(), ucp->MetaClass, *ucp->MetaClass->GetName());
					}
				}
				++It2;
			}
			PyErr_Format(PyExc_Exception, "function %s signature's is not compatible with the parent", name);
			return nullptr;
		}
	}

	function->ParmsSize = 0;
	function->NumParms = 0;

	// allocate properties storage (ignore super)
	TFieldIterator<UProperty> props(function, EFieldIteratorFlags::ExcludeSuper);
	for (; props; ++props)
	{
		UProperty* p = *props;
		if (p->HasAnyPropertyFlags(CPF_Parm))
		{
			function->NumParms++;
			function->ParmsSize = p->GetOffset_ForUFunction() + p->GetSize();
			if (p->HasAnyPropertyFlags(CPF_ReturnParm))
			{
				function->ReturnValueOffset = p->GetOffset_ForUFunction();
			}
		}
	}

	if (parent_function)
	{
		UE_LOG(LogPython, Warning, TEXT("OVERRIDDEN FUNCTION %s WITH %d PARAMS (size %d) %d"), *function->GetFName().ToString(), function->NumParms, function->ParmsSize, function->PropertiesSize);
	}
	else
	{
		UE_LOG(LogPython, Warning, TEXT("REGISTERED FUNCTION %s WITH %d PARAMS (size %d) %d"), *function->GetFName().ToString(), function->NumParms, function->ParmsSize, function->PropertiesSize);
	}


#if ENGINE_MINOR_VERSION >= 17
	function->FunctionFlags = (EFunctionFlags)function_flags;
#else
	function->FunctionFlags = function_flags;
#endif

#if ENGINE_MINOR_VERSION > 18
	function->SetNativeFunc((FNativeFuncPtr)& UPythonFunction::CallPythonCallable);
#else
	function->SetNativeFunc((Native)& UPythonFunction::CallPythonCallable);
#endif

	function->Next = u_class->Children;



	u_class->Children = function;
#if ENGINE_MINOR_VERSION < 18
	u_class->AddFunctionToFunctionMap(function);
#else
	u_class->AddFunctionToFunctionMap(function, function->GetFName());
#endif

	u_class->Bind();
	u_class->StaticLink(true);

	// regenerate CDO
	u_class->GetDefaultObject()->RemoveFromRoot();
	u_class->GetDefaultObject()->ConditionalBeginDestroy();
	u_class->ClassDefaultObject = nullptr;

#if WITH_EDITOR
	u_class->PostEditChange();
#endif

	u_class->GetDefaultObject()->PostInitProperties();

#if WITH_EDITOR
	u_class->PostLinkerChange();
#endif

#if WITH_EDITOR
	// this is required for avoiding startup crashes #405
	if (GEditor)
	{
		FBlueprintActionDatabase::Get().RefreshClassActions(u_class);
	}
#endif

	return function;
}

FGuid* ue_py_check_fguid(PyObject* py_obj)
{
	ue_PyUScriptStruct* ue_py_struct = py_ue_is_uscriptstruct(py_obj);
	if (!ue_py_struct)
	{
		return nullptr;
	}

	if (ue_py_struct->u_struct == FindObject<UScriptStruct>(ANY_PACKAGE, UTF8_TO_TCHAR((char*)"Guid")))
	{
		return (FGuid*)ue_py_struct->u_struct_ptr;
	}

	return nullptr;
}
uint8* do_ue_py_check_struct(PyObject* py_obj, UScriptStruct* chk_u_struct)
{
	ue_PyUScriptStruct* ue_py_struct = py_ue_is_uscriptstruct(py_obj);
	if (!ue_py_struct)
	{
		return nullptr;
	}

	if (ue_py_struct->u_struct == chk_u_struct)
	{
		return ue_py_struct->u_struct_ptr;
	}

	return nullptr;
}

bool do_ue_py_check_childstruct(PyObject* py_obj, UScriptStruct* parent_u_struct)
{
	ue_PyUScriptStruct* ue_py_struct = py_ue_is_uscriptstruct(py_obj);
	if (!ue_py_struct)
	{
		return false;
	}

	return ue_py_struct->u_struct->IsChildOf(parent_u_struct);
}



#if PY_MAJOR_VERSION >= 3
static PyObject * init_unreal_engine()
{

	PyObject* new_unreal_engine_module = PyModule_Create(&unreal_engine_module);
	if (!new_unreal_engine_module)
		return nullptr;

	return new_unreal_engine_module;
}
#endif
