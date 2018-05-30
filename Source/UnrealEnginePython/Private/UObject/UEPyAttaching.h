#pragma once



#include "UEPyModule.h"

PyObject *py_ue_get_socket_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_socket_rotation(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_socket_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_socket_world_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_socket_actor_transform(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_attached_actors(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_all_child_actors(ue_PyUObject *, PyObject *);
PyObject *py_ue_attach_to_actor(ue_PyUObject *, PyObject *);
PyObject *py_ue_attach_to_component(ue_PyUObject *, PyObject *);