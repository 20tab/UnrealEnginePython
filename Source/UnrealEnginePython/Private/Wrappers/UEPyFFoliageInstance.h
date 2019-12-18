#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR
#include "InstancedFoliage.h"

typedef struct 
{
	PyObject_HEAD
	TWeakObjectPtr<AInstancedFoliageActor> foliage_actor;
	TWeakObjectPtr<UFoliageType> foliage_type;
	int32 instance_id;
} ue_PyFFoliageInstance;

void ue_python_init_ffoliage_instance(PyObject *);

PyObject *py_ue_new_ffoliage_instance(AInstancedFoliageActor *foliage_actor, UFoliageType *foliage_type, int32 instance_id);
#endif
