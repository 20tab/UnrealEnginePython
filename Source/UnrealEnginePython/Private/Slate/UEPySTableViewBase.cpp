

#include "UEPySTableViewBase.h"


static PyObject *py_ue_stable_view_base_set_item_height(ue_PySTableViewBase *self, PyObject * args)
{
	ue_py_slate_cast(STableViewBase);
	float size;
	if (!PyArg_ParseTuple(args, "f:set_item_height", &size))
	{
		return nullptr;
	}

	py_STableViewBase->SetItemHeight(size);

	Py_RETURN_SLATE_SELF;
}


static PyObject *py_ue_stable_view_base_set_item_width(ue_PySTableViewBase *self, PyObject * args)
{
	ue_py_slate_cast(STableViewBase);
	float size;
	if (!PyArg_ParseTuple(args, "f:set_item_width", &size))
	{
		return nullptr;
	}

	py_STableViewBase->SetItemWidth(size);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_stable_view_base_request_list_refresh(ue_PySTableViewBase *self, PyObject * args)
{
	ue_py_slate_cast(STableViewBase);
	py_STableViewBase->RequestListRefresh();

	Py_RETURN_NONE;
}

static PyMethodDef ue_PySTableViewBase_methods[] = {
	{ "set_item_height", (PyCFunction)py_ue_stable_view_base_set_item_height, METH_VARARGS, "" },
	{ "set_item_width", (PyCFunction)py_ue_stable_view_base_set_item_width, METH_VARARGS, "" },
	{ "request_list_refresh", (PyCFunction)py_ue_stable_view_base_request_list_refresh, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySTableViewBaseType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.STableViewBase", /* tp_name */
	sizeof(ue_PySTableViewBase), /* tp_basicsize */
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
	"Unreal Engine STableViewBase",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySTableViewBase_methods,             /* tp_methods */
};

void ue_python_init_stable_view_base(PyObject *ue_module)
{

	ue_PySTableViewBaseType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySTableViewBaseType) < 0)
		return;

	Py_INCREF(&ue_PySTableViewBaseType);
	PyModule_AddObject(ue_module, "STableViewBase", (PyObject *)&ue_PySTableViewBaseType);
}

ue_PySTableViewBase * py_ue_is_stable_view_base(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySTableViewBaseType))
		return nullptr;
	return (ue_PySTableViewBase *)obj;
}
