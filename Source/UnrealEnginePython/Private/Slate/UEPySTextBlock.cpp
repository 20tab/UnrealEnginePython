#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySTextBlock.h"



#define GET_s_text_block STextBlock *s_text_block = (STextBlock *)self->s_leaf_widget.s_widget.s_widget

static PyObject *py_ue_stext_block_set_text(ue_PySTextBlock *self, PyObject * args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text)) {
		return NULL;
	}

	GET_s_text_block;

	s_text_block->SetText(FString(UTF8_TO_TCHAR(text)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_PySTextBlock_str(ue_PySTextBlock *self)
{
	return PyUnicode_FromFormat("<unreal_engine.STextBlock '%p'>",
		self->s_leaf_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySTextBlock_methods[] = {
	{ "set_text", (PyCFunction)py_ue_stext_block_set_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySTextBlockType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.STextBlock", /* tp_name */
	sizeof(ue_PySTextBlock), /* tp_basicsize */
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
	(reprfunc)ue_PySTextBlock_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine STextBlock",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySTextBlock_methods,             /* tp_methods */
};

static int ue_py_stext_block_init(ue_PySTextBlock *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(STextBlock, s_leaf_widget.s_widget);
	return 0;
}

void ue_python_init_stext_block(PyObject *ue_module) {
	ue_PySTextBlockType.tp_new = PyType_GenericNew;

	ue_PySTextBlockType.tp_init = (initproc)ue_py_stext_block_init;

	ue_PySTextBlockType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySTextBlockType) < 0)
		return;

	Py_INCREF(&ue_PySTextBlockType);
	PyModule_AddObject(ue_module, "STextBlock", (PyObject *)&ue_PySTextBlockType);
}


#endif