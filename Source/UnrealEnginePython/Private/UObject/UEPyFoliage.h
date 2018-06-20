#pragma once

#include "UEPyModule.h"

PyObject *py_ue_get_instanced_foliage_actor_for_current_level(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_instanced_foliage_actor_for_level(ue_PyUObject *, PyObject *);
PyObject *py_ue_get_foliage_types(ue_PyUObject *, PyObject *);
#if WITH_EDITOR
PyObject *py_ue_get_foliage_instances(ue_PyUObject *, PyObject *);
PyObject *py_ue_add_foliage_asset(ue_PyUObject *, PyObject *);
#endif
