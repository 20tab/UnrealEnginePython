#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySImage.h"

#define GET_s_image SImage *s_image = (SImage *)self->s_leaf_widget.s_widget.s_widget

static PyObject *py_ue_simage_set_brush(ue_PySImage *self, PyObject * args) {
	PyObject *py_brush;
	if (!PyArg_ParseTuple(args, "O:set_brush", &py_brush)) {
		return NULL;
	}

	FSlateBrush *brush = ue_py_check_struct<FSlateBrush>(py_brush);
	if (!brush)
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateBrush");

	GET_s_image;

	s_image->SetImage(brush);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_PySImage_str(ue_PySImage *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SImage '%p'>",
		self->s_leaf_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySImage_methods[] = {
	{ "set_brush", (PyCFunction)py_ue_simage_set_brush, METH_VARARGS, "" },
	{ "set_image", (PyCFunction)py_ue_simage_set_brush, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySImageType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SImage", /* tp_name */
	sizeof(ue_PySImage), /* tp_basicsize */
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
	(reprfunc)ue_PySImage_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SImage",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySImage_methods,             /* tp_methods */
};

static int ue_py_simage_init(ue_PySImage *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SImage, s_leaf_widget.s_widget);
	return 0;
}

void ue_python_init_simage(PyObject *ue_module) {
	ue_PySImageType.tp_new = PyType_GenericNew;

	ue_PySImageType.tp_init = (initproc)ue_py_simage_init;

	ue_PySImageType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySImageType) < 0)
		return;

	Py_INCREF(&ue_PySImageType);
	PyModule_AddObject(ue_module, "SImage", (PyObject *)&ue_PySImageType);
}


#endif