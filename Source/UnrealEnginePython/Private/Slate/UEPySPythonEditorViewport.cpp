#if WITH_EDITOR
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonEditorViewport.h"



#define GET_s_python_editor_viewport SPythonEditorViewport *s_python_editor_viewport = (SPythonEditorViewport *)self->s_editor_viewport.s_compound_widget.s_widget.s_widget

static PyObject *py_ue_spython_editor_viewport_get_world(ue_PySPythonEditorViewport *self, PyObject * args) {

	GET_s_python_editor_viewport;

	ue_PyUObject *ret = ue_get_python_wrapper(s_python_editor_viewport->GetPythonWorld());
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

static PyObject *py_ue_spython_editor_viewport_set_show_bounds(ue_PySPythonEditorViewport *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_bounds", &py_bool)) {
		return NULL;
	}

	GET_s_python_editor_viewport;

	s_python_editor_viewport->GetViewportClient()->SetShowBounds(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_spython_editor_viewport_set_show_stats(ue_PySPythonEditorViewport *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_show_stats", &py_bool)) {
		return NULL;
	}

	GET_s_python_editor_viewport;

	s_python_editor_viewport->GetViewportClient()->SetShowStats(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_spython_editor_viewport_set_view_mode(ue_PySPythonEditorViewport *self, PyObject * args) {
	int mode;
	if (!PyArg_ParseTuple(args, "i:set_view_mode", &mode)) {
		return NULL;
	}

	GET_s_python_editor_viewport;

	s_python_editor_viewport->GetViewportClient()->SetViewMode((EViewModeIndex)mode);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_spython_editor_viewport_set_exposure_settings(ue_PySPythonEditorViewport *self, PyObject * args) {
	PyObject *py_settings;
	if (!PyArg_ParseTuple(args, "O:set_exposure_settings", &py_settings)) {
		return NULL;
	}

	FExposureSettings *settings = ue_py_check_struct<FExposureSettings>(py_settings);
	if (!settings) {
		return PyErr_Format(PyExc_Exception, "argument is not a FExposureSettings");
	}

	GET_s_python_editor_viewport;

	s_python_editor_viewport->GetViewportClient()->ExposureSettings = *settings;

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_PySPythonEditorViewport_str(ue_PySPythonEditorViewport *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SPythonEditorViewport '%p'>",
		self->s_editor_viewport.s_compound_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySPythonEditorViewport_methods[] = {
	{ "get_world", (PyCFunction)py_ue_spython_editor_viewport_get_world, METH_VARARGS, "" },
	{ "set_show_bounds", (PyCFunction)py_ue_spython_editor_viewport_set_show_bounds, METH_VARARGS, "" },
	{ "set_show_stats", (PyCFunction)py_ue_spython_editor_viewport_set_show_stats, METH_VARARGS, "" },
	{ "set_view_mode", (PyCFunction)py_ue_spython_editor_viewport_set_view_mode, METH_VARARGS, "" },
	{ "set_exposure_settings", (PyCFunction)py_ue_spython_editor_viewport_set_exposure_settings, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

UWorld *SPythonEditorViewport::GetPythonWorld() {
	return GetWorld();
}

TSharedRef<FEditorViewportClient> SPythonEditorViewport::MakeEditorViewportClient() {
	TSharedPtr<FEditorViewportClient> client = MakeShareable(new FEditorViewportClient(nullptr, new FPreviewScene(), SharedThis(this)));
	client->SetRealtime(true);

	return client.ToSharedRef();
}

PyTypeObject ue_PySPythonEditorViewportType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonEditorViewport", /* tp_name */
	sizeof(ue_PySPythonEditorViewport), /* tp_basicsize */
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
	(reprfunc)ue_PySPythonEditorViewport_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SPythonEditorViewport",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonEditorViewport_methods,             /* tp_methods */
};

static int ue_py_spython_editor_viewport_init(ue_PySPythonEditorViewport *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SPythonEditorViewport, s_editor_viewport.s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_spython_editor_viewport(PyObject *ue_module) {
	ue_PySPythonEditorViewportType.tp_new = PyType_GenericNew;

	ue_PySPythonEditorViewportType.tp_init = (initproc)ue_py_spython_editor_viewport_init;

	ue_PySPythonEditorViewportType.tp_base = &ue_PySEditorViewportType;

	if (PyType_Ready(&ue_PySPythonEditorViewportType) < 0)
		return;

	Py_INCREF(&ue_PySPythonEditorViewportType);
	PyModule_AddObject(ue_module, "SPythonEditorViewport", (PyObject *)&ue_PySPythonEditorViewportType);
}
#endif