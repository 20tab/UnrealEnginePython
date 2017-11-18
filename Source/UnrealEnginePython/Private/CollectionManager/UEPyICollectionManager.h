#pragma once

#if WITH_EDITOR

#include "UnrealEnginePython.h"

#include "Developer/CollectionManager/Public/ICollectionManager.h"


typedef struct
{
	PyObject_HEAD
	/* Type-specific fields go here. */
} ue_PyICollectionManager;

void ue_python_init_icollection_manager(PyObject *);

#endif
