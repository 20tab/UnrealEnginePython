#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_actor_has_tag(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_begin_play(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_bounds(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_component(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_destroy(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_components(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_velocity(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_set_level_sequence(ue_PyUObject *, PyObject *);
#if WITH_EDITOR
PyObject *py_ue_get_actor_label(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_actor_label(ue_PyUObject *, PyObject *);
PyObject *py_ue_find_actor_by_label(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_editor_world_counterpart_actor(ue_PyUObject *, PyObject *);
#endif
PyObject *py_ue_get_owner(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_actor_component(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_python_component(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_actor_root_component(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_has_component_of_type(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_component_by_type(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_components_by_type(ue_PyUObject *, PyObject *);
PyObject *py_ue_actor_spawn(ue_PyUObject * self, PyObject *, PyObject *);
PyObject *py_ue_get_overlapping_actors(ue_PyUObject *, PyObject *);

PyObject *py_ue_actor_destroy_component(ue_PyUObject *, PyObject *);

PyObject *py_ue_register_component(ue_PyUObject * self, PyObject *);
PyObject *py_ue_unregister_component(ue_PyUObject * self, PyObject *);
PyObject *py_ue_destroy_component(ue_PyUObject * self, PyObject *);
PyObject *py_ue_component_is_registered(ue_PyUObject * self, PyObject *);
PyObject *py_ue_actor_create_default_subobject(ue_PyUObject * self, PyObject *);