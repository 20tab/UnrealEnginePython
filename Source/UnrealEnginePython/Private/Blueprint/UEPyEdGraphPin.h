#pragma once

#include "UEPyModule.h"

#if WITH_EDITOR



#include "Runtime/Engine/Classes/EdGraph/EdGraphPin.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		UEdGraphPin *pin;
} ue_PyEdGraphPin;

PyObject *py_ue_new_edgraphpin(UEdGraphPin *);
ue_PyEdGraphPin *py_ue_is_edgraphpin(PyObject *);

void ue_python_init_edgraphpin(PyObject *);
#endif