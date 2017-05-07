#pragma once



#include "UnrealEnginePython.h"

PyObject *py_write_audio_to_buffer(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_play_sound_at_location(ue_PyUObject *, PyObject *);