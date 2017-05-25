#pragma once
#include "UnrealEnginePython.h"

#if WITH_EDITOR

struct ue_PyFAssetData {
	PyObject_HEAD
		/* Type-specific fields go here. */
		FAssetData *asset_data;
	PyObject *asset_class;
	PyObject *asset_name;
	PyObject *group_names;
	PyObject *object_path;
	PyObject *package_flags;
	PyObject *package_name;
	PyObject *package_path;
	PyObject *tags_and_values;
};

PyObject *py_ue_new_fassetdata(FAssetData *);
ue_PyFAssetData *py_ue_is_fassetdata(PyObject *);

void ue_python_init_fassetdata(PyObject *);

#endif
