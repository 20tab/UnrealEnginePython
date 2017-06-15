#pragma once



#include "UnrealEnginePython.h"

#include "Runtime/CoreUObject/Public/Misc/StringAssetReference.h"

typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
	FStringAssetReference fstring_asset_reference;
} ue_PyFStringAssetReference;

PyObject *py_ue_new_fstring_asset_reference(FStringAssetReference);
ue_PyFStringAssetReference *py_ue_is_fstring_asset_reference(PyObject *);

void ue_python_init_fstring_asset_reference(PyObject *);