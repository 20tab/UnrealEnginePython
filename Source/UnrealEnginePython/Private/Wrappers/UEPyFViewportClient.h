#pragma once

#include "UEPyModule.h"
#include "UnrealClient.h"

extern PyTypeObject ue_PyFViewportClientType;

struct ue_PyFViewportClient
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		TSharedRef<FViewportClient> viewport_client;;
};

void ue_python_init_fviewport_client(PyObject *);

PyObject *py_ue_new_fviewport_client(TSharedRef<FViewportClient>);

ue_PyFViewportClient *py_ue_is_fviewport_client(PyObject *);
