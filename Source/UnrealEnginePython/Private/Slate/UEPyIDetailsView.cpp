
#include "UEPyIDetailsView.h"

#if WITH_EDITOR

#include "Editor/PropertyEditor/Public/IDetailsView.h"




static PyObject *py_ue_idetails_view_set_object(ue_PyIDetailsView *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(IDetailsView);

	PyObject *py_in_obj = nullptr;
	PyObject *py_force_refresh = nullptr;

	char *kwlist[] = {
		(char *)"uobject",
		(char *)"force_refresh",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:set_object", kwlist, &py_in_obj, &py_force_refresh))
	{
		return nullptr;
	}

	UObject *u_object = ue_py_check_type<UObject>(py_in_obj);
	if (!u_object)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	const bool bForceRefresh = py_force_refresh && PyObject_IsTrue(py_force_refresh);

	py_IDetailsView->SetObject(u_object, bForceRefresh);

	Py_RETURN_NONE;
}

static PyMethodDef ue_PyIDetailsView_methods[] = {
#pragma warning(suppress: 4191)
	{ "set_object", (PyCFunction)py_ue_idetails_view_set_object, METH_VARARGS | METH_KEYWORDS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PyIDetailsViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IDetailsView", /* tp_name */
	sizeof(ue_PyIDetailsView), /* tp_basicsize */
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
	"Unreal Engine IDetailsView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIDetailsView_methods,             /* tp_methods */
};

void ue_python_init_idetails_view(PyObject *ue_module)
{
	ue_PyIDetailsViewType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PyIDetailsViewType) < 0)
		return;

	Py_INCREF(&ue_PyIDetailsViewType);
	PyModule_AddObject(ue_module, "IDetailsView", (PyObject *)&ue_PyIDetailsViewType);
}

ue_PyIDetailsView *py_ue_is_idetails_view(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyIDetailsViewType))
		return nullptr;
	return (ue_PyIDetailsView *)obj;
}
#endif