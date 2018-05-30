#pragma once

#include "UnrealEnginePython.h"

#if ENGINE_MINOR_VERSION <= 17
#include "Runtime/Core/Public/GenericPlatform/GenericApplication.h"
#else
#include "Runtime/ApplicationCore/Public/GenericPlatform/GenericApplication.h"
#endif

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FModifierKeysState modifier;
} ue_PyFModifierKeysState;

void ue_python_init_fmodifier_keys_state(PyObject *);

PyObject *py_ue_new_fmodifier_keys_state(FModifierKeysState);
ue_PyFModifierKeysState *py_ue_is_fmodifier_keys_state(PyObject *);