#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

static PyObject *py_ue_feditor_viewport_client_take_high_res_screen_shot(ue_PyFEditorViewportClient *self, PyObject * args)
{
	self->editor_viewport_client->TakeHighResScreenShot();
	self->editor_viewport_client->Invalidate();
	if (self->editor_viewport_client->Viewport)
	{
		FIntPoint point = self->editor_viewport_client->Viewport->GetSizeXY();
		if (point.X > 0 && point.Y > 0)
		{
			self->editor_viewport_client->Viewport->Draw();
		}
	}
	Py_RETURN_NONE;
}

static PyObject *py_ue_feditor_viewport_client_tick(ue_PyFEditorViewportClient *self, PyObject * args)
{
	float delta_seconds = 0;
	if (!PyArg_ParseTuple(args, "|f:tick", &delta_seconds))
		return nullptr;
	self->editor_viewport_client->Tick(delta_seconds);
	Py_RETURN_NONE;
}

static PyMethodDef ue_PyFEditorViewportClient_methods[] = {
	{ "take_high_res_screen_shot", (PyCFunction)py_ue_feditor_viewport_client_take_high_res_screen_shot, METH_VARARGS, "" },
	{ "tick", (PyCFunction)py_ue_feditor_viewport_client_tick, METH_VARARGS, "" },
	{ nullptr }  /* Sentinel */
};

static PyObject *ue_PyFEditorViewportClient_str(ue_PyFEditorViewportClient *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FEditorViewportClient %p>",
		&self->editor_viewport_client.Get());
}

static PyTypeObject ue_PyFEditorViewportClientType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FEditorViewportClient", /* tp_name */
	sizeof(ue_PyFEditorViewportClient), /* tp_basicsize */
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
	(reprfunc)ue_PyFEditorViewportClient_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
#if PY_MAJOR_VERSION < 3
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_CHECKTYPES,        /* tp_flags */
#else
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
#endif
	"Unreal Engine FEditorViewportClient",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFEditorViewportClient_methods,             /* tp_methods */
	0,
};

void ue_python_init_feditor_viewport_client(PyObject *ue_module)
{
	ue_PyFEditorViewportClientType.tp_new = PyType_GenericNew;
	ue_PyFEditorViewportClientType.tp_base = &ue_PyFViewportClientType;

	if (PyType_Ready(&ue_PyFEditorViewportClientType) < 0)
		return;

	Py_INCREF(&ue_PyFEditorViewportClientType);
	PyModule_AddObject(ue_module, "FEditorViewportClient", (PyObject *)&ue_PyFEditorViewportClientType);
}

ue_PyFEditorViewportClient *py_ue_is_feditor_viewport_client(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFEditorViewportClientType))
		return nullptr;
	return (ue_PyFEditorViewportClient *)obj;
}

PyObject *py_ue_new_feditor_viewport_client(TSharedRef<FEditorViewportClient> editor_viewport_client)
{
	ue_PyFEditorViewportClient *ret = (ue_PyFEditorViewportClient *)PyObject_New(ue_PyFEditorViewportClient, &ue_PyFEditorViewportClientType);
	new(&ret->viewport_client.viewport_client) TSharedRef<FViewportClient>(editor_viewport_client);
	new(&ret->editor_viewport_client) TSharedRef<FEditorViewportClient>(editor_viewport_client);
	return (PyObject *)ret;
}
#endif