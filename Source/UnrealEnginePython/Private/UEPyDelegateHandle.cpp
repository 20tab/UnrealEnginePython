#include "UnrealEnginePythonPrivatePCH.h"
#include "UEPyDelegateHandle.h"

static PyMethodDef ue_PyFDelegateHandle_methods[] = {{ NULL }};

// destructor
static void ue_pyfdelegatehandle_dealloc(ue_PyFDelegateHandle *self) {
	Py_DECREF(self->py_callable);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyFDelegateHandleType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FDelegateHandle", /* tp_name */
	sizeof(ue_PyFDelegateHandle), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_pyfdelegatehandle_dealloc,       /* tp_dealloc */
	0,                         /* tp_print */
	0,                         /* tp_getattr */
	0,                         /* tp_setattr */
	0,                         /* tp_reserved */
	0,                         /* tp_repr */
	0,                         /* tp_as_number */
	0,                         /* tp_as_sequence */
	0,                         /* tp_as_mapping */
	0,                         /* tp_hash  */
	0,                         /* tp_call */
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FDelegateHandle",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFDelegateHandle_methods,             /* tp_methods */
};

void ue_python_init_fdelegatehandle(PyObject *ue_module) {
	ue_PyFDelegateHandleType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ue_PyFDelegateHandleType) < 0)
		return;

	Py_INCREF(&ue_PyFDelegateHandleType);
	PyModule_AddObject(ue_module, "FDelegateHandle", (PyObject *)&ue_PyFDelegateHandleType);
}
