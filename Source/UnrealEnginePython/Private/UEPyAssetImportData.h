#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
typedef struct {
	PyObject_HEAD
		/* Type-specific fields go here. */
		UAssetImportData *asset_import_data;
	PyObject *source_data;
} ue_PyAssetImportData;

PyObject *py_ue_new_assetimportdata(UAssetImportData *);
ue_PyAssetImportData *py_ue_is_assetimportdata(PyObject *);
void ue_python_init_assetimportdata(PyObject *);

PyObject *py_ue_get_assetimportdata(ue_PyUObject *, PyObject *);
#endif
