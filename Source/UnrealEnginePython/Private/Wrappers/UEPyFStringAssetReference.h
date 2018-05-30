#pragma once



#include "UEPyModule.h"

#if ENGINE_MINOR_VERSION < 18
#include "Runtime/CoreUObject/Public/Misc/StringAssetReference.h"
#endif

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FStringAssetReference fstring_asset_reference;
} ue_PyFStringAssetReference;

PyObject *py_ue_new_fstring_asset_reference(FStringAssetReference);
ue_PyFStringAssetReference *py_ue_is_fstring_asset_reference(PyObject *);

void ue_python_init_fstring_asset_reference(PyObject *);