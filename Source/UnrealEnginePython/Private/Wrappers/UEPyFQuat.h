#pragma once

#include "UnrealEnginePython.h"

#include "Runtime/Core/Public/Math/Quat.h"

extern PyTypeObject ue_PyUScriptStructType;

typedef struct {
    ue_PyUScriptStruct py_base;
} ue_PyFQuat;

PyObject *py_ue_new_fquat(const FQuat&);
PyObject *py_ue_new_fquat_ptr(FQuat*);
ue_PyFQuat *py_ue_is_fquat(PyObject *);

inline static FQuat& py_ue_fquat_get(ue_PyFQuat *self)
{
    return *((FQuat*)py_ue_uscriptstruct_get_data((ue_PyUScriptStruct *)self));
}

void ue_python_init_fquat(PyObject *);

bool py_ue_quat_arg(PyObject *, FQuat &);