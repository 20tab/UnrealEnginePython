#pragma once



#include "UnrealEnginePython.h"

PyObject *py_ue_sequencer_tracks(ue_PyUObject *, PyObject *);
#if WITH_EDITOR
PyObject *py_ue_sequencer_folders(ue_PyUObject *, PyObject *);
#endif
PyObject *py_ue_sequencer_sections(ue_PyUObject *, PyObject *);
PyObject *py_ue_sequencer_possessables(ue_PyUObject *, PyObject *);