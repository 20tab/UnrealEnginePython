#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"



#include "UEPySlate.h"


void ue_py_init_slate(PyObject *module) {
	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
}


#endif