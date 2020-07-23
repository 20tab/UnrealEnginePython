#pragma once



#include "UEPyModule.h"

#if ENGINE_MINOR_VERSION >= 25

#include "Runtime/CoreUObject/Public/UObject/Field.h"

// as properties no longer UObjects they wont get imported by the UObjects classes importer

// - this does mean all scripts will need some editing
// - for the moment have to live with this until have some working properties

// as now there is no longer any way I can see so far of getting a list of property classes
// defined we will have to have wrappers for each known property class
// WRONG - there is apparently a list of FFieldClass classes accessed either as
// FFieldClass::GetAllFieldClasses() or a TMap FFieldClass::GetNameToFieldClassMap()

typedef struct
{
        PyObject_HEAD
                /* Type-specific fields go here. */
} ue_PyFPropertiesImporter;

PyObject *py_ue_new_fpropertiesimporter();

void ue_python_init_fpropertiesimporter(PyObject *);


#endif
