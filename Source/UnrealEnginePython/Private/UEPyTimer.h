#pragma once



#include "UEPyModule.h"
#include "Engine/EngineTypes.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		FTimerHandle thandle;
	TWeakObjectPtr<UWorld> world;
	TSharedPtr<FPythonSmartDelegate> delegate_ptr;
} ue_PyFTimerHandle;

PyObject *py_ue_set_timer(ue_PyUObject *, PyObject *);

void ue_python_init_ftimerhandle(PyObject *);