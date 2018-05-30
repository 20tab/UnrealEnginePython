
#include "UEPySNodePanel.h"

#if WITH_EDITOR


static PyMethodDef ue_PySNodePanel_methods[] = {
	{ NULL }  /* Sentinel */
};


PyTypeObject ue_PySNodePanelType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SNodePanel", /* tp_name */
	sizeof(ue_PySNodePanel), /* tp_basicsize */
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
	"Unreal Engine SNode Panel",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySNodePanel_methods,             /* tp_methods */
};

void ue_python_init_snode_panel(PyObject *ue_module)
{

	ue_PySNodePanelType.tp_base = &ue_PySPanelType;

	if (PyType_Ready(&ue_PySNodePanelType) < 0)
		return;

	Py_INCREF(&ue_PySNodePanelType);
	PyModule_AddObject(ue_module, "SNodePanel", (PyObject *)&ue_PySNodePanelType);
}

#endif