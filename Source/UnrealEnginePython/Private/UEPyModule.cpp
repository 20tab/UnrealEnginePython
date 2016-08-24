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
#if WITH_EDITOR
#include "UEPyEditor.h"
#endif



DEFINE_LOG_CATEGORY(LogPython);

PyDoc_STRVAR(unreal_engine_py_doc, "Unreal Engine Python module.");

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


static PyMethodDef unreal_engine_methods[] = {
	{ "log", py_unreal_engine_log, METH_VARARGS, "" },
	{ "log_warning", py_unreal_engine_log_warning, METH_VARARGS, "" },
	{ "log_error", py_unreal_engine_log_error, METH_VARARGS, "" },
	{ "add_on_screen_debug_message", py_unreal_engine_add_on_screen_debug_message, METH_VARARGS, "" },
	{ "print_string", py_unreal_engine_print_string, METH_VARARGS, "" },

	{ "find_class", py_unreal_engine_find_class, METH_VARARGS, "" },
	{ "find_object", py_unreal_engine_find_object, METH_VARARGS, "" },
	{ "vector_add_vector", py_unreal_engine_vector_add_vector, METH_VARARGS, "" },
	{ "vector_add_float", py_unreal_engine_vector_add_float, METH_VARARGS, "" },
	{ "vector_mul_vector", py_unreal_engine_vector_mul_vector, METH_VARARGS, "" },
	{ "vector_mul_float", py_unreal_engine_vector_mul_float, METH_VARARGS, "" },

	{ "get_forward_vector", py_unreal_engine_get_forward_vector, METH_VARARGS, "" },
	{ "get_up_vector", py_unreal_engine_get_up_vector, METH_VARARGS, "" },
	{ "get_right_vector", py_unreal_engine_get_right_vector, METH_VARARGS, "" },

	{ "color_to_linear", py_unreal_engine_color_to_linear, METH_VARARGS, "" },
	{ "color_from_linear", py_unreal_engine_color_from_linear, METH_VARARGS, "" },

#if WITH_EDITOR
	{ "get_editor_world", py_unreal_engine_get_editor_world, METH_VARARGS, "" },
	{ "editor_get_selected_actors", (PyCFunction)py_unreal_engine_editor_get_selected_actors, METH_VARARGS, "" },
	{ "editor_select_actor", (PyCFunction)py_unreal_engine_editor_select_actor, METH_VARARGS, "" },
	{ "editor_deselect_actors", (PyCFunction)py_unreal_engine_editor_deselect_actors, METH_VARARGS, "" },
#endif

	{ NULL, NULL },
};



static PyObject *py_ue_set_simulate_physics(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	bool enabled = true;

	PyObject *is_true = NULL;
	if (!PyArg_ParseTuple(args, "|O:set_simulate_physics", &is_true)) {
		return NULL;
	}

	if (is_true && !PyObject_IsTrue(is_true))
		enabled = false;

	UPrimitiveComponent *primitive = nullptr;

	if (self->ue_object->IsA<UPrimitiveComponent>()) {
		primitive = (UPrimitiveComponent *)self->ue_object;
	}
	else {
		return PyErr_Format(PyExc_Exception, "uobject is not an UPrimitiveComponent");
	}

	if (!primitive) {
		return PyErr_Format(PyExc_Exception, "unable to set physics for the object");
	}

	primitive->SetSimulatePhysics(enabled);

	Py_INCREF(Py_None);
	return Py_None;
}





static PyObject *py_ue_destructible_apply_damage(ue_PyUObject *, PyObject *);
static PyObject *py_ue_set_view_target(ue_PyUObject *, PyObject *);

static PyObject *py_ue_play_sound_at_location(ue_PyUObject *, PyObject *);


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
	{ "get_properties", (PyCFunction)py_ue_properties, METH_VARARGS, "" },

	{ "is_a", (PyCFunction)py_ue_is_a, METH_VARARGS, "" },

	{ "call", (PyCFunction)py_ue_call, METH_VARARGS, "" },
	{ "get_owner", (PyCFunction)py_ue_get_owner, METH_VARARGS, "" },
	{ "get_name", (PyCFunction)py_ue_get_name, METH_VARARGS, "" },
	{ "get_full_name", (PyCFunction)py_ue_get_full_name, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "get_actor_label", (PyCFunction)py_ue_get_actor_label, METH_VARARGS, "" },
	{ "set_actor_label", (PyCFunction)py_ue_set_actor_label, METH_VARARGS, "" },

	{ "find_actor_by_label", (PyCFunction)py_ue_find_actor_by_label, METH_VARARGS, "" },
#endif


	{ "find_function", (PyCFunction)py_ue_find_function, METH_VARARGS, "" },
	{ "call_function", (PyCFunction)py_ue_call_function, METH_VARARGS, "" },


	{ "all_objects", (PyCFunction)py_ue_all_objects, METH_VARARGS, "" },
	{ "all_actors", (PyCFunction)py_ue_all_actors, METH_VARARGS, "" },

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

	{ "quit_game", (PyCFunction)py_ue_quit_game, METH_VARARGS, "" },
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

	{ "add_actor_component", (PyCFunction)py_ue_add_actor_component, METH_VARARGS, "" },
	{ "add_actor_root_component", (PyCFunction)py_ue_add_actor_root_component, METH_VARARGS, "" },
	{ "get_actor_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component_by_type", (PyCFunction)py_ue_get_actor_component_by_type, METH_VARARGS, "" },
	{ "get_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_component", (PyCFunction)py_ue_get_actor_component, METH_VARARGS, "" },
	{ "get_actor_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },
	{ "get_components_by_type", (PyCFunction)py_ue_get_actor_components_by_type, METH_VARARGS, "" },

	{ "set_simulate_physics", (PyCFunction)py_ue_set_simulate_physics, METH_VARARGS, "" },
	{ "get_world", (PyCFunction)py_ue_get_world, METH_VARARGS, "" },

	{ "get_world_location_at_distance_along_spline", (PyCFunction)py_ue_get_world_location_at_distance_along_spline, METH_VARARGS, "" },
	{ "get_spline_length", (PyCFunction)py_ue_get_spline_length, METH_VARARGS, "" },

	{ "get_actor_velocity", (PyCFunction)py_ue_get_actor_velocity, METH_VARARGS, "" },

	{ "play_sound_at_location", (PyCFunction)py_ue_play_sound_at_location, METH_VARARGS, "" },

	{ "world_tick", (PyCFunction)py_ue_world_tick, METH_VARARGS, "" },

	{ "create_player", (PyCFunction)py_ue_create_player, METH_VARARGS, "" },
	{ "get_num_players", (PyCFunction)py_ue_get_num_players, METH_VARARGS, "" },
	{ "get_num_spectators", (PyCFunction)py_ue_get_num_spectators, METH_VARARGS, "" },

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


	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyUObjectType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UObject",             /* tp_name */
	sizeof(ue_PyUObject), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,                         /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
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



static PyObject *py_ue_play_sound_at_location(ue_PyUObject *self, PyObject * args) {

	ue_py_check(self);

	PyObject *sound;
	float x, y, z;
	float volume = 1;
	float pitch = 1;
	float start = 0;

	if (!PyArg_ParseTuple(args, "Offf|fff:play_sound_at_location", &sound, &x, &y, &z, &volume, &pitch, &start)) {
		return NULL;
	}


	USoundBase *sound_object = nullptr;
	if (PyObject_IsInstance(sound, (PyObject *)&ue_PyUObjectType)) {
		ue_PyUObject *py_sound = (ue_PyUObject *)sound;
		if (py_sound->ue_object->IsA<USoundBase>()) {
			sound_object = (USoundBase *)py_sound->ue_object;
		}
	}
	else if (PyUnicode_Check(sound)) {
		sound_object = FindObject<USoundBase>(ANY_PACKAGE, UTF8_TO_TCHAR(PyUnicode_AsUTF8(sound)));
	}

	if (!sound_object)
		return PyErr_Format(PyExc_Exception, "invalid sound object");

	UGameplayStatics::PlaySoundAtLocation(self->ue_object, sound_object, FVector(x, y, z), volume, pitch, start);

	Py_INCREF(Py_None);
	return Py_None;
}



static PyObject *py_ue_set_view_target(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	UWorld *world = ue_get_uworld(self);
	if (!world)
		return PyErr_Format(PyExc_Exception, "unable to retrieve UWorld from uobject");

	PyObject *obj;
	int controller_id = 0;
	if (!PyArg_ParseTuple(args, "O|i:set_view_target", &obj, &controller_id)) {
		return NULL;
	}

	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUObjectType)) {
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	ue_PyUObject *py_obj = (ue_PyUObject *)obj;

	if (!py_obj->ue_object->IsA<AActor>()) {
		return PyErr_Format(PyExc_Exception, "argument is not an actor");
	}

	AActor *actor = (AActor *)py_obj->ue_object;

	APlayerController *controller = UGameplayStatics::GetPlayerController(world, controller_id);
	if (!controller)
		return PyErr_Format(PyExc_Exception, "unable to retrieve controller %d", controller_id);

	controller->SetViewTarget(actor);

	Py_INCREF(Py_None);
	return Py_None;

}





static PyObject *py_ue_destructible_apply_damage(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

	float damage_amount = 0;
	float impulse_strength = 0;
	float x = 0, y = 0, z = 0;
	float ix = 0, iy = 0, iz = 0;
	if (!PyArg_ParseTuple(args, "ffffffff:destructible_apply_damage", &damage_amount, &impulse_strength, &x, &y, &z, &ix, &iy, &iz)) {
		return NULL;
	}

	UDestructibleComponent *destructible = nullptr;
	AActor *actor = nullptr;

	if (self->ue_object->IsA<UDestructibleComponent>()) {
		destructible = (UDestructibleComponent *)self->ue_object;
	}
	else if (self->ue_object->IsA<AActor>()) {
		actor = (AActor *)self->ue_object;
		destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
	}
	else if (self->ue_object->IsA<UActorComponent>()) {
		actor = (AActor *)self->ue_object->GetOuter();
		if (actor) {
			destructible = (UDestructibleComponent *)actor->GetComponentByClass(UDestructibleComponent::StaticClass());
		}
	}

	if (destructible) {
		destructible->ApplyDamage(damage_amount, FVector(x, y, z), FVector(ix, iy, iz), impulse_strength);
	}
	else {
		return PyErr_Format(PyExc_Exception, "UObject is not a destructible");
	}

	Py_INCREF(Py_False);
	return Py_False;
}

void unreal_engine_init_py_module() {
	PyImport_AppendInittab("unreal_engine", init_unreal_engine);
	PyObject *new_unreal_engine_module = PyImport_AddModule("unreal_engine");

	PyObject *unreal_engine_dict = PyModule_GetDict(new_unreal_engine_module);

	PyMethodDef *unreal_engine_function;
	for (unreal_engine_function = unreal_engine_methods; unreal_engine_function->ml_name != NULL; unreal_engine_function++) {
		PyObject *func = PyCFunction_New(unreal_engine_function, NULL);
		PyDict_SetItemString(unreal_engine_dict, unreal_engine_function->ml_name, func);
		Py_DECREF(func);
	}


	ue_PyUObjectType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ue_PyUObjectType) < 0)
		return;

	Py_INCREF(&ue_PyUObjectType);
	PyModule_AddObject(new_unreal_engine_module, "UObject", (PyObject *)&ue_PyUObjectType);

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


	// Colors
	PyDict_SetItemString(unreal_engine_dict, "COLOR_BLACK", Py_BuildValue("iii", FColor::Black.R, FColor::Black.G, FColor::Black.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_BLUE", Py_BuildValue("iii", FColor::Blue.R, FColor::Blue.G, FColor::Blue.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_CYAN", Py_BuildValue("iii", FColor::Cyan.R, FColor::Cyan.G, FColor::Cyan.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_EMERALD", Py_BuildValue("iii", FColor::Emerald.R, FColor::Emerald.G, FColor::Emerald.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_GREEN", Py_BuildValue("iii", FColor::Green.R, FColor::Green.G, FColor::Green.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_MAGENTA", Py_BuildValue("iii", FColor::Magenta.R, FColor::Magenta.G, FColor::Magenta.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_ORANGE", Py_BuildValue("iii", FColor::Orange.R, FColor::Orange.G, FColor::Orange.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_PURPLE", Py_BuildValue("iii", FColor::Purple.R, FColor::Purple.G, FColor::Purple.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_RED", Py_BuildValue("iii", FColor::Red.R, FColor::Red.G, FColor::Red.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_SILVER", Py_BuildValue("iii", FColor::Silver.R, FColor::Silver.G, FColor::Silver.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_TURQUOISE", Py_BuildValue("iii", FColor::Turquoise.R, FColor::Turquoise.G, FColor::Turquoise.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_WHITE", Py_BuildValue("iii", FColor::White.R, FColor::White.G, FColor::White.B));
	PyDict_SetItemString(unreal_engine_dict, "COLOR_YELLOW", Py_BuildValue("iii", FColor::Yellow.R, FColor::Yellow.G, FColor::Yellow.B));

}


// utility functions

ue_PyUObject *ue_get_python_wrapper(UObject *ue_obj) {
	if (!ue_obj || !ue_obj->IsValidLowLevel() || ue_obj->IsPendingKillOrUnreachable())
		return nullptr;
	UPyObject *ue_py_object = FindObject<UPyObject>(ue_obj, TEXT("__PyObject"), true);
	if (!ue_py_object) {
		ue_py_object = NewObject<UPyObject>(ue_obj, FName(TEXT("__PyObject")));
		if (!ue_py_object) {
			return nullptr;
		}
		ue_py_object->py_object = (ue_PyUObject *)PyObject_New(ue_PyUObject, &ue_PyUObjectType);
		if (!ue_py_object->py_object) {
			ue_py_object->ConditionalBeginDestroy();
			return nullptr;
		}
		((ue_PyUObject *)ue_py_object->py_object)->ue_object = ue_obj;


		Py_INCREF(ue_py_object->py_object);
	}

	return ue_py_object->py_object;
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
	PyObject *zero = PyUnicode_AsUTF8String(PyObject_Str(value));
	if (zero) {
		msg = PyBytes_AsString(zero);
	}

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

	if (auto casted_prop = Cast<UFloatProperty>(prop)) {
		float value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyFloat_FromDouble(value);
	}

	if (auto casted_prop = Cast<UStrProperty>(prop)) {
		FString value = casted_prop->GetPropertyValue_InContainer(buffer);
		return PyUnicode_FromString(TCHAR_TO_UTF8(*value));
	}

	if (auto casted_prop = Cast<UObjectProperty>(prop)) {
		auto value = casted_prop->GetPropertyValue_InContainer(buffer);
		if (value) {
			ue_PyUObject *ret = ue_get_python_wrapper(value);
			if (!ret)
				return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
			Py_INCREF(ret);
			return (PyObject *)ret;
		}
		goto end;
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
		goto end;
	}

	return PyErr_Format(PyExc_Exception, "unsupported value type for property %s", TCHAR_TO_UTF8(*prop->GetName()));

end:
	Py_INCREF(Py_None);
	return Py_None;
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

	if (PyLong_Check(py_obj)) {
		auto casted_prop = Cast<UIntProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, PyLong_AsLong(py_obj));
		return true;
	}

	if (PyFloat_Check(py_obj)) {
		auto casted_prop = Cast<UFloatProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, PyFloat_AsDouble(py_obj));
		return true;
	}

	if (PyUnicode_Check(py_obj)) {
		auto casted_prop = Cast<UStrProperty>(prop);
		if (!casted_prop)
			return false;
		casted_prop->SetPropertyValue_InContainer(buffer, FString(UTF8_TO_TCHAR(PyUnicode_AsUTF8(py_obj))));
		return true;
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

		if (ue_obj->ue_object->IsA<UObjectProperty>()) {
			auto casted_prop = Cast<UObjectProperty>(prop);
			if (!casted_prop)
				return false;
			casted_prop->SetPropertyValue_InContainer(buffer, ue_obj->ue_object);
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
