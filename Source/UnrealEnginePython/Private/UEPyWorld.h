#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_quit_game(ue_PyUObject *, PyObject *);

// mainly used for unit testing
PyObject *py_ue_world_tick(ue_PyUObject *, PyObject *);


PyObject *py_ue_all_objects(ue_PyUObject *, PyObject *);
PyObject *py_ue_all_actors(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_world(ue_PyUObject *, PyObject *);