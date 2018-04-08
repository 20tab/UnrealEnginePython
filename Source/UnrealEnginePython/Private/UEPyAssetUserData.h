#pragma once



#include "UEPyModule.h"

#if WITH_EDITOR
#include "EditorFramework/AssetImportData.h"
PyObject *py_ue_asset_import_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_asset_import_data_set_sources(ue_PyUObject *, PyObject *);
#endif
