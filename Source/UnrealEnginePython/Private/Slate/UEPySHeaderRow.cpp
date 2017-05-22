#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySHeaderRow.h"

#define GET_s_header_row SHeaderRow *s_header_row =(SHeaderRow *)self->s_border.s_compound_widget.s_widget.s_widget

static PyObject *ue_PySHeaderRow_str(ue_PySButton *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SButton '%p'>",
		self->s_border.s_compound_widget.s_widget.s_widget);
}


static PyMethodDef ue_PySHeaderRow_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySHeaderRowType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SHeaderRow", /* tp_name */
	sizeof(ue_PySHeaderRow), /* tp_basicsize */
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
	(reprfunc)ue_PySHeaderRow_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SHeaderRow",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySHeaderRow_methods,             /* tp_methods */
};

static int ue_py_sheader_row_init(ue_PySHeaderRow *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SHeaderRow, s_border.s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sheader_row(PyObject *ue_module) {
	ue_PySButtonType.tp_new = PyType_GenericNew;

	ue_PySButtonType.tp_init = (initproc)ue_py_sheader_row_init;

	ue_PySButtonType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySHeaderRowType) < 0)
		return;

	Py_INCREF(&ue_PySHeaderRowType);
	PyModule_AddObject(ue_module, "SHeaderRow", (PyObject *)&ue_PySHeaderRowType);
}


#endif