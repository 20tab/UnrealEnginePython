#pragma once



#include "UEPyModule.h"

PyObject *py_ue_get_actor_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_transform(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_actor_forward(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_right(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_actor_up(ue_PyUObject *, PyObject *);



PyObject *py_ue_set_actor_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_actor_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_actor_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_actor_transform(ue_PyUObject *, PyObject *);

PyObject *py_ue_add_actor_world_offset(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_actor_local_offset(ue_PyUObject *, PyObject *);

PyObject *py_ue_add_actor_world_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_actor_local_rotation(ue_PyUObject *, PyObject *);


PyObject *py_ue_get_world_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_relative_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_relative_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_relative_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_relative_transform(ue_PyUObject *, PyObject *);

PyObject *py_ue_set_world_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_world_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_world_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_world_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_relative_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_relative_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_relative_scale(ue_PyUObject *, PyObject *);
PyObject *py_ue_set_relative_transform(ue_PyUObject *, PyObject *);

PyObject *py_ue_get_forward_vector(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_up_vector(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_right_vector(ue_PyUObject *, PyObject *);