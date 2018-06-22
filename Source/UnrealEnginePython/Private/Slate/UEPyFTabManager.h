#pragma once

#include "UEPyModule.h"

#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"

typedef struct
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		TSharedRef<FTabManager> tab_manager;
} ue_PyFTabManager;

PyObject *py_ue_new_ftab_manager(TSharedRef<FTabManager>);

void ue_python_init_ftab_manager(PyObject *);
