
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySSplitter.h"

#define GET_s_splitter SSplitter *s_splitter = (SSplitter *)self->s_panel.s_widget.s_widget

static PyObject *py_ue_ssplitter_add_slot(ue_PySSplitter *self, PyObject * args) {
	PyObject *py_content;
	int index = -1;
	if (!PyArg_ParseTuple(args, "O|i:add_slot", &py_content, &index)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	// TODO: decrement reference when destroying parent
	Py_INCREF(py_swidget);

	GET_s_splitter;

	SSplitter::FSlot &fslot = s_splitter->AddSlot(index);
	fslot.AttachWidget(py_swidget->s_widget->AsShared());

	Py_INCREF(self);
	return (PyObject *)self;
}


static PyObject *ue_PySSplitter_str(ue_PySSplitter *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SSplitter '%p'>",
		self->s_panel.s_widget.s_widget);
}

static PyMethodDef ue_PySSplitter_methods[] = {
	{ "add_slot", (PyCFunction)py_ue_ssplitter_add_slot, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySSplitterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SSplitter", /* tp_name */
	sizeof(ue_PySSplitter), /* tp_basicsize */
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
	(reprfunc)ue_PySSplitter_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SSplitter",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySSplitter_methods,             /* tp_methods */
};

static int ue_py_ssplitter_init(ue_PySSplitter *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SSplitter, s_panel.s_widget);
	return 0;
}

void ue_python_init_ssplitter(PyObject *ue_module) {
	ue_PySSplitterType.tp_new = PyType_GenericNew;

	ue_PySSplitterType.tp_init = (initproc)ue_py_ssplitter_init;

	ue_PySSplitterType.tp_base = &ue_PySPanelType;

	if (PyType_Ready(&ue_PySSplitterType) < 0)
		return;

	Py_INCREF(&ue_PySSplitterType);
	PyModule_AddObject(ue_module, "SSplitter", (PyObject *)&ue_PySSplitterType);
}
