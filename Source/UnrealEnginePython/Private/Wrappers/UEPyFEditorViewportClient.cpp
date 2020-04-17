#include "UEPyFEditorViewportClient.h"

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

static PyObject *py_ue_feditor_viewport_client_get_look_at_location(ue_PyFEditorViewportClient *self, PyObject * args)
{
	return py_ue_new_fvector(self->editor_viewport_client->GetLookAtLocation());
}

static PyObject *py_ue_feditor_viewport_client_get_view_location(ue_PyFEditorViewportClient *self, PyObject * args)
{
	return py_ue_new_fvector(self->editor_viewport_client->GetViewLocation());
}

static PyObject *py_ue_feditor_viewport_client_get_view_rotation(ue_PyFEditorViewportClient *self, PyObject * args)
{
	return py_ue_new_frotator(self->editor_viewport_client->GetViewRotation());
}

static PyObject *py_ue_feditor_viewport_client_get_camera_speed(ue_PyFEditorViewportClient *self, PyObject * args)
{
	return PyFloat_FromDouble(self->editor_viewport_client->GetCameraSpeed());
}

static PyObject *py_ue_feditor_viewport_client_get_viewport_dimensions(ue_PyFEditorViewportClient *self, PyObject * args)
{
	FIntPoint origin;
	FIntPoint size;
	self->editor_viewport_client->GetViewportDimensions(origin, size);
	return Py_BuildValue((char *)"(ii)(ii)", origin.X, origin.Y, size.X, size.Y);
}

static PyObject *py_ue_feditor_viewport_client_is_visible(ue_PyFEditorViewportClient *self, PyObject * args)
{
	if (self->editor_viewport_client->IsVisible())
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_feditor_viewport_client_get_scene_depth_at_location(ue_PyFEditorViewportClient *self, PyObject * args)
{
	int x;
	int y;
	if (!PyArg_ParseTuple(args, "ii:get_scene_depth_at_location", &x, &y))
		return nullptr;
	return PyFloat_FromDouble(self->editor_viewport_client->GetSceneDepthAtLocation(x, y));
}

static PyObject *py_ue_feditor_viewport_client_set_look_at_location(ue_PyFEditorViewportClient *self, PyObject * args)
{
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	self->editor_viewport_client->SetLookAtLocation(vec);
	Py_RETURN_NONE;
}

static PyObject *py_ue_feditor_viewport_client_set_view_location(ue_PyFEditorViewportClient *self, PyObject * args)
{
	FVector vec;
	if (!py_ue_vector_arg(args, vec))
		return PyErr_Format(PyExc_Exception, "argument is not a FVector");
	self->editor_viewport_client->SetViewLocation(vec);
	Py_RETURN_NONE;
}

static PyObject *py_ue_feditor_viewport_client_set_view_rotation(ue_PyFEditorViewportClient *self, PyObject * args)
{
	FRotator rot;
	if (!py_ue_rotator_arg(args, rot))
		return PyErr_Format(PyExc_Exception, "argument is not a FRotator");
	self->editor_viewport_client->SetViewRotation(rot);
	Py_RETURN_NONE;
}

static PyObject *py_ue_feditor_viewport_client_set_realtime(ue_PyFEditorViewportClient *self, PyObject * args)
{
	PyObject* bInRealtime;
	PyObject* bStoreCurrentValue;
	if (!PyArg_ParseTuple(args, "OO", &bInRealtime, &bStoreCurrentValue))
		return nullptr;

	self->editor_viewport_client->SetRealtime(PyObject_IsTrue(bInRealtime) ? true : false,
		PyObject_IsTrue(bStoreCurrentValue) ? true : false);
	Py_RETURN_NONE;
}

static PyMethodDef ue_PyFEditorViewportClient_methods[] = {
	{ "take_high_res_screen_shot", (PyCFunction)py_ue_feditor_viewport_client_take_high_res_screen_shot, METH_VARARGS, "" },
	{ "tick", (PyCFunction)py_ue_feditor_viewport_client_tick, METH_VARARGS, "" },
	{ "get_look_at_location", (PyCFunction)py_ue_feditor_viewport_client_get_look_at_location, METH_VARARGS, "" },
	{ "get_view_location", (PyCFunction)py_ue_feditor_viewport_client_get_view_location, METH_VARARGS, "" },
	{ "get_view_rotation", (PyCFunction)py_ue_feditor_viewport_client_get_view_location, METH_VARARGS, "" },
	{ "get_camera_speed", (PyCFunction)py_ue_feditor_viewport_client_get_camera_speed, METH_VARARGS, "" },
	{ "get_viewport_dimensions", (PyCFunction)py_ue_feditor_viewport_client_get_viewport_dimensions, METH_VARARGS, "" },
	{ "is_visible", (PyCFunction)py_ue_feditor_viewport_client_is_visible, METH_VARARGS, "" },
	{ "get_scene_depth_at_location", (PyCFunction)py_ue_feditor_viewport_client_get_scene_depth_at_location, METH_VARARGS, "" },
	{ "set_look_at_location", (PyCFunction)py_ue_feditor_viewport_client_set_look_at_location, METH_VARARGS, "" },
	{ "set_view_location", (PyCFunction)py_ue_feditor_viewport_client_set_view_location, METH_VARARGS, "" },
	{ "set_view_rotation", (PyCFunction)py_ue_feditor_viewport_client_set_view_rotation, METH_VARARGS, "" },
	{ "set_realtime", (PyCFunction)py_ue_feditor_viewport_client_set_realtime, METH_VARARGS, "" },
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