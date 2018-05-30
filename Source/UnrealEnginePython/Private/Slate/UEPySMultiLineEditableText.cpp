

#include "UEPySMultiLineEditableText.h"


static PyObject *py_ue_smulti_line_editable_text_select_all_text(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);

	py_SMultiLineEditableText->SelectAllText();

	Py_RETURN_NONE;
}

static PyObject *py_ue_smulti_line_editable_text_clear_selection(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);

	py_SMultiLineEditableText->ClearSelection();

	Py_RETURN_NONE;
}

static PyObject *py_ue_smulti_line_editable_text_get_selected_text(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);

	FText text = py_SMultiLineEditableText->GetSelectedText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_get_text(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);

	FText text = py_SMultiLineEditableText->GetText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_get_plain_text(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);

	FText text = py_SMultiLineEditableText->GetPlainText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_smulti_line_editable_text_set_text(ue_PySMultiLineEditableText *self, PyObject * args)
{
	ue_py_slate_cast(SMultiLineEditableText);
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text))
	{
		return NULL;
	}

	py_SMultiLineEditableText->SetText(FText::FromString(UTF8_TO_TCHAR(text)));

	Py_RETURN_SLATE_SELF;
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
	0,                         /* tp_str */
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

static int ue_py_smulti_line_editable_text_init(ue_PySMultiLineEditableText *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SMultiLineEditableText);

	ue_py_slate_farguments_optional_bool("allow_context_menu", AllowContextMenu);
	ue_py_slate_farguments_optional_bool("auto_wrap_text", AutoWrapText);
	ue_py_slate_farguments_optional_bool("is_read_only", IsReadOnly);
	ue_py_slate_farguments_optional_struct_ptr("text_style", TextStyle, FTextBlockStyle);

	ue_py_slate_farguments_text("text", Text);

	ue_py_snew(SMultiLineEditableText);
	return 0;
}

void ue_python_init_smulti_line_editable_text(PyObject *ue_module)
{

	ue_PySMultiLineEditableTextType.tp_init = (initproc)ue_py_smulti_line_editable_text_init;

	ue_PySMultiLineEditableTextType.tp_base = &ue_PySWidgetType;

	if (PyType_Ready(&ue_PySMultiLineEditableTextType) < 0)
		return;

	Py_INCREF(&ue_PySMultiLineEditableTextType);
	PyModule_AddObject(ue_module, "SMultiLineEditableText", (PyObject *)&ue_PySMultiLineEditableTextType);
}
