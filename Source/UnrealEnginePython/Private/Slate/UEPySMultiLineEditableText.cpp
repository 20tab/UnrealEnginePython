#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySEditableTextBox.h"



#define GET_s_multi_line_editable_text SMultiLineEditableText *s_multi_line_editable_text = (SMultiLineEditableText *)self->s_widget.s_widget

static PyObject *py_ue_smulti_line_editable_text_select_all_text(ue_PySMultiLineEditableText *self, PyObject * args) {
	GET_s_multi_line_editable_text;

	s_multi_line_editable_text->SelectAllText();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_smulti_line_editable_text_clear_selection(ue_PySMultiLineEditableText *self, PyObject * args) {
	GET_s_multi_line_editable_text;

	s_multi_line_editable_text->ClearSelection();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_smulti_line_editable_text_get_selected_text(ue_PySMultiLineEditableText *self, PyObject * args) {
	GET_s_multi_line_editable_text;

	FText text = s_multi_line_editable_text->GetSelectedText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_get_text(ue_PySMultiLineEditableText *self, PyObject * args) {
	GET_s_multi_line_editable_text;

	FText text = s_multi_line_editable_text->GetText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_get_plain_text(ue_PySMultiLineEditableText *self, PyObject * args) {
	GET_s_multi_line_editable_text;

	FText text = s_multi_line_editable_text->GetPlainText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_set_text(ue_PySMultiLineEditableText *self, PyObject * args) {
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text)) {
		return NULL;
	}

	GET_s_multi_line_editable_text;

	s_multi_line_editable_text->SetText(FText::FromString(UTF8_TO_TCHAR(text)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_PySMultiLineEditableText_str(ue_PySMultiLineEditableText *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SMultiLineEditableText '%p'>",
		self->s_widget.s_widget);
}



static PyMethodDef ue_PySMultiLineEditableText_methods[] = {
	{ "select_all_text", (PyCFunction)py_ue_smulti_line_editable_text_select_all_text, METH_VARARGS, "" },
	{ "clear_selection", (PyCFunction)py_ue_smulti_line_editable_text_clear_selection, METH_VARARGS, "" },
	{ "get_selected_text", (PyCFunction)py_ue_smulti_line_editable_text_get_selected_text, METH_VARARGS, "" },
	{ "get_text", (PyCFunction)py_ue_smulti_line_editable_text_get_text, METH_VARARGS, "" },
	{ "set_text", (PyCFunction)py_ue_smulti_line_editable_text_set_text, METH_VARARGS, "" },
	{ "get_plain_text", (PyCFunction)py_ue_smulti_line_editable_text_get_plain_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySMultiLineEditableTextType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SMultiLineEditableText", /* tp_name */
	sizeof(ue_PySMultiLineEditableText), /* tp_basicsize */
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
	(reprfunc)ue_PySMultiLineEditableText_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SMultiLineEditableText",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySMultiLineEditableText_methods,             /* tp_methods */
};

static int ue_py_smulti_line_editable_text_init(ue_PySMultiLineEditableText *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SMultiLineEditableText, s_widget);
	return 0;
}

void ue_python_init_smulti_line_editable_text(PyObject *ue_module) {
	ue_PySMultiLineEditableTextType.tp_new = PyType_GenericNew;

	ue_PySMultiLineEditableTextType.tp_init = (initproc)ue_py_smulti_line_editable_text_init;

	ue_PySMultiLineEditableTextType.tp_base = &ue_PySWidgetType;

	if (PyType_Ready(&ue_PySMultiLineEditableTextType) < 0)
		return;

	Py_INCREF(&ue_PySMultiLineEditableTextType);
	PyModule_AddObject(ue_module, "SMultiLineEditableText", (PyObject *)&ue_PySMultiLineEditableTextType);
}


#endif