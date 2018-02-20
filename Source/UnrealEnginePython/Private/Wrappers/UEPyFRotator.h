#pragma once



#include "UnrealEnginePython.h"

extern PyTypeObject ue_PyUScriptStructType;

typedef struct {
    ue_PyUScriptStruct py_base;
} ue_PyFRotator;

PyObject *py_ue_new_frotator(const FRotator&);
PyObject *py_ue_new_frotator_ptr(FRotator*);
ue_PyFRotator *py_ue_is_frotator(PyObject *);

inline static FRotator& py_ue_frotator_get(ue_PyFRotator *self)
{
    return *((FRotator*)py_ue_uscriptstruct_get_data((ue_PyUScriptStruct *)self));
}

void ue_python_init_frotator(PyObject *);

bool py_ue_rotator_arg(PyObject *, FRotator &);