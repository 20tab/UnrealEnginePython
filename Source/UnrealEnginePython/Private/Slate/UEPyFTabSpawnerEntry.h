#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FTabSpawnerEntry *spawner_entry;
} ue_PyFTabSpawnerEntry;

PyObject *py_ue_new_ftab_spawner_entry(FTabSpawnerEntry *);

void ue_python_init_ftab_spawner_entry(PyObject *);
