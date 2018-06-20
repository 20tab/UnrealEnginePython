#pragma once
#include "UnrealEnginePython.h"

#include "InstancedFoliage.h"

typedef struct 
{
	PyObject_HEAD
	FFoliageInstance instance;
} ue_PyFFoliageInstance;

void ue_python_init_ffoliage_instance(PyObject *);

PyObject *py_ue_new_ffoliage_instance(FFoliageInstance instance);
