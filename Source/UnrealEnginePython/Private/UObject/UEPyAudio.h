#pragma once



#include "UEPyModule.h"

PyObject *py_ue_queue_audio(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_play_sound_at_location(ue_PyUObject *, PyObject *);
PyObject *py_ue_sound_get_data(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_sound_set_data(ue_PyUObject *self, PyObject * args);
PyObject *py_ue_get_available_audio_byte_count(ue_PyUObject *, PyObject *);
PyObject *py_ue_reset_audio(ue_PyUObject *, PyObject *);