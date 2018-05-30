#pragma once

#include "UEPyModule.h"

#if WITH_EDITOR

struct ue_PyFAssetData
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FAssetData asset_data;
};

PyObject *py_ue_new_fassetdata(FAssetData);
ue_PyFAssetData *py_ue_is_fassetdata(PyObject *);

void ue_python_init_fassetdata(PyObject *);

#endif
