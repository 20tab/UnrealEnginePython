#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_create_player(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_num_players(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_num_spectators(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_player_controller(ue_PyUObject *, PyObject *);