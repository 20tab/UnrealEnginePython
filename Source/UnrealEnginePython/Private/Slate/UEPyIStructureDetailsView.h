#pragma once

#include "UnrealEnginePython.h"

#if WITH_EDITOR
typedef struct {
	PyObject_HEAD
	/* Type-specific fields go here. */
    TSharedPtr<IStructureDetailsView> istructure_details_view;
    ue_PyUScriptStruct *ue_py_struct;
} ue_PyIStructureDetailsView;

void ue_python_init_istructure_details_view(PyObject *);

ue_PyIStructureDetailsView * py_ue_is_istructure_details_view(PyObject *obj);

#endif