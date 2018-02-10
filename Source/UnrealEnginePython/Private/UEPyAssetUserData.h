#pragma once



#include "UnrealEnginePython.h"

#if WITH_EDITOR
PyObject *py_ue_asset_import_data(ue_PyUObject *, PyObject *);
PyObject *py_ue_asset_import_data_set_sources(ue_PyUObject *, PyObject *);
#endif
