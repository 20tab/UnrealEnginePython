

#include "UEPySListView.h"

static PyMethodDef ue_PySListView_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySListViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SListView", /* tp_name */
	sizeof(ue_PySListView), /* tp_basicsize */
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
	"Unreal Engine SListView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySListView_methods,             /* tp_methods */
};


void ue_python_init_slist_view(PyObject *ue_module)
{

	ue_PySListViewType.tp_base = &ue_PySTableViewBaseType;

	if (PyType_Ready(&ue_PySListViewType) < 0)
		return;

	Py_INCREF(&ue_PySListViewType);
	PyModule_AddObject(ue_module, "SListView", (PyObject *)&ue_PySListViewType);
}
