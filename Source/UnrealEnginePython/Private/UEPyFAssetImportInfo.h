#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
typedef struct {
	PyObject_HEAD
		/* Type-specific fields go here. */
		FAssetImportInfo *asset_import_info;
	PyObject *source_files;
} ue_PyFAssetImportInfo;

PyObject *py_ue_new_fassetimportinfo(FAssetImportInfo *);
ue_PyFAssetImportInfo *py_ue_is_fassetimportinfo(PyObject *);
void ue_python_init_fassetimportinfo(PyObject *);

typedef struct {
	PyObject_HEAD
		/* Type-specific fields go here. */
		FAssetImportInfo::FSourceFile *source_file;
} ue_PyFSourceFile;

PyObject *py_ue_new_fsourcefile(FAssetImportInfo::FSourceFile *);
ue_PyFSourceFile *py_ue_is_fsourcefile(PyObject *);
#endif
