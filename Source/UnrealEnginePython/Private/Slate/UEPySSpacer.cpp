#include "UEPySSpacer.h"

static PyMethodDef ue_PySSpacer_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySSpacerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SSpacer", /* tp_name */
	sizeof(ue_PySSpacer), /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,       /* tp_dealloc */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SSpacer",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySSpacer_methods,             /* tp_methods */
};

static int ue_py_sspacer_init(ue_PySSpacer *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SSpacer);

	ue_py_slate_farguments_fvector2d("size", Size);

	ue_py_snew(SSpacer);
	return 0;
}

void ue_python_init_sspacer(PyObject *ue_module)
{

	ue_PySSpacerType.tp_init = (initproc)ue_py_sspacer_init;

	ue_PySSpacerType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySSpacerType) < 0)
		return;

	Py_INCREF(&ue_PySSpacerType);
	PyModule_AddObject(ue_module, "SSpacer", (PyObject *)&ue_PySSpacerType);
}
