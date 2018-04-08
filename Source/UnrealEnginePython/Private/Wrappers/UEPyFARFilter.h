#pragma once
#include "UEPyModule.h"

#if WITH_EDITOR

#include "AssetRegistryModule.h"
#include "Runtime/AssetRegistry/Public/ARFilter.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FARFilter filter;
	PyObject *class_names;
	PyObject *object_paths;
	PyObject *package_names;
	PyObject *package_paths;
	PyObject *recursive_classes_exclusion_set;
	PyObject *tags_and_values;
} ue_PyFARFilter;

PyObject *py_ue_new_farfilter(FARFilter);
ue_PyFARFilter *py_ue_is_farfilter(PyObject *);

void ue_python_init_farfilter(PyObject *);
void py_ue_sync_farfilter(PyObject *);
void py_ue_clear_farfilter(ue_PyFARFilter *);

#endif
