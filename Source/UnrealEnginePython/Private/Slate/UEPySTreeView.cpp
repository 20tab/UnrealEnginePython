
#include "UEPySTreeView.h"

static PyObject *py_ue_stree_view_request_tree_refresh(ue_PySTreeView *self, PyObject * args)
{
	ue_py_slate_cast_named(STreeView<TSharedPtr<void>>, py_STreeView);
	py_STreeView->RequestTreeRefresh();
	Py_RETURN_NONE;
}

static PyObject *py_ue_stree_view_clear_expanded_items(ue_PySTreeView *self, PyObject * args)
{
	ue_py_slate_cast_named(STreeView<TSharedPtr<void>>, py_STreeView);
	py_STreeView->ClearExpandedItems();
	Py_RETURN_NONE;
}


static PyMethodDef ue_PySTreeView_methods[] = {
	{ "request_tree_refresh", (PyCFunction)py_ue_stree_view_request_tree_refresh, METH_VARARGS, "" },
	{ "clear_expanded_items", (PyCFunction)py_ue_stree_view_clear_expanded_items, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySTreeViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.STreeView", /* tp_name */
	sizeof(ue_PySTreeView), /* tp_basicsize */
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
	"Unreal Engine STreeView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySTreeView_methods,             /* tp_methods */
};


void ue_python_init_stree_view(PyObject *ue_module)
{

	ue_PySTreeViewType.tp_base = &ue_PySListViewType;

	if (PyType_Ready(&ue_PySTreeViewType) < 0)
		return;

	Py_INCREF(&ue_PySTreeViewType);
	PyModule_AddObject(ue_module, "STreeView", (PyObject *)&ue_PySTreeViewType);
}
