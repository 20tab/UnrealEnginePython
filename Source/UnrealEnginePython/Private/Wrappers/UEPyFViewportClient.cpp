#include "UEPyFViewportClient.h"

#include "Engine/World.h"

static PyObject *py_ue_fviewport_client_get_world(ue_PyFViewportClient *self, PyObject * args)
{
	UWorld *u_world = self->viewport_client->GetWorld();
	if (!u_world)
		return PyErr_Format(PyExc_Exception, "unable to get UWorld");
	Py_RETURN_UOBJECT(u_world);
}

static PyObject *py_ue_fviewport_client_is_ortho(ue_PyFViewportClient *self, PyObject * args)
{
	if (self->viewport_client->IsOrtho())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_fviewport_client_is_in_game_view(ue_PyFViewportClient *self, PyObject * args)
{
	if (self->viewport_client->IsInGameView())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}


static PyMethodDef ue_PyFViewportClient_methods[] = {
	{ "get_world", (PyCFunction)py_ue_fviewport_client_get_world, METH_VARARGS, "" },
	{ "is_in_game_view", (PyCFunction)py_ue_fviewport_client_is_in_game_view, METH_VARARGS, "" },
	{ "is_ortho", (PyCFunction)py_ue_fviewport_client_is_ortho, METH_VARARGS, "" },
	{ nullptr }  /* Sentinel */
};

static PyObject *ue_PyFViewportClient_str(ue_PyFViewportClient *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FViewportClient %p>",
		&self->viewport_client.Get());
}

PyTypeObject ue_PyFViewportClientType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FViewportClient", /* tp_name */
	sizeof(ue_PyFViewportClient), /* tp_basicsize */
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
	(reprfunc)ue_PyFViewportClient_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
#endif
	"Unreal Engine FViewportClient",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFViewportClient_methods,             /* tp_methods */
	0,
};

void ue_python_init_fviewport_client(PyObject *ue_module)
{
	ue_PyFViewportClientType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFViewportClientType) < 0)
		return;

	Py_INCREF(&ue_PyFViewportClientType);
	PyModule_AddObject(ue_module, "FViewportClient", (PyObject *)&ue_PyFViewportClientType);
}

ue_PyFViewportClient *py_ue_is_fviewport_client(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFViewportClientType))
		return nullptr;
	return (ue_PyFViewportClient *)obj;
}

PyObject *py_ue_new_fviewport_client(TSharedRef<FViewportClient> viewport_client)
{
	ue_PyFViewportClient *ret = (ue_PyFViewportClient *)PyObject_New(ue_PyFViewportClient, &ue_PyFViewportClientType);
	new(&ret->viewport_client) TSharedRef<FViewportClient>(viewport_client);
	return (PyObject *)ret;
}