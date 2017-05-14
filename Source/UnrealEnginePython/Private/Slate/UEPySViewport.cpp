#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySViewport.h"



#define GET_s_viewport SViewport *s_viewport = (SViewport*) self->s_compound_widget.s_widget.s_widget

static PyObject *py_ue_sviewport_enable_stereo_rendering(ue_PySViewport *self, PyObject * args) {
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:enable_stereo_rendering", &py_bool)) {
		return NULL;
	}

	GET_s_viewport;

	s_viewport->EnableStereoRendering(PyObject_IsTrue(py_bool) ? true : false);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_PySViewport_str(ue_PySViewport *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SViewport '%p'>",
		self->s_compound_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySViewport_methods[] = {
	{ "enable_stereo_rendering", (PyCFunction)py_ue_sviewport_enable_stereo_rendering, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySViewportType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SViewport", /* tp_name */
	sizeof(ue_PySViewport), /* tp_basicsize */
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
	(reprfunc)ue_PySViewport_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SViewport",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySViewport_methods,             /* tp_methods */
};

static int ue_py_sviewport_init(ue_PySViewport *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SViewport, s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sviewport(PyObject *ue_module) {
	ue_PySViewportType.tp_new = PyType_GenericNew;

	ue_PySViewportType.tp_init = (initproc)ue_py_sviewport_init;

	ue_PySViewportType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySViewportType) < 0)
		return;

	Py_INCREF(&ue_PySViewportType);
	PyModule_AddObject(ue_module, "SViewport", (PyObject *)&ue_PySViewportType);
}


#endif