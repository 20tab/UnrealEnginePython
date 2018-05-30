


#include "UEPySEditableTextBox.h"


static PyObject *py_ue_seditable_text_box_select_all_text(ue_PySEditableTextBox *self, PyObject * args)
{
	ue_py_slate_cast(SEditableTextBox);

	py_SEditableTextBox->SelectAllText();

	Py_RETURN_NONE;
}

static PyObject *py_ue_seditable_text_box_clear_selection(ue_PySEditableTextBox *self, PyObject * args)
{
	ue_py_slate_cast(SEditableTextBox);

	py_SEditableTextBox->ClearSelection();

	Py_RETURN_NONE;
}

static PyObject *py_ue_seditable_text_box_get_selected_text(ue_PySEditableTextBox *self, PyObject * args)
{
	ue_py_slate_cast(SEditableTextBox);

	FText text = py_SEditableTextBox->GetSelectedText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_seditable_text_box_get_text(ue_PySEditableTextBox *self, PyObject * args)
{
	ue_py_slate_cast(SEditableTextBox);

	FText text = py_SEditableTextBox->GetText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_seditable_text_box_set_text(ue_PySEditableTextBox *self, PyObject * args)
{
	ue_py_slate_cast(SEditableTextBox);
	char *text;
	if (!PyArg_ParseTuple(args, "s:set_text", &text))
	{
		return nullptr;
	}

	py_SEditableTextBox->SetText(FText::FromString(UTF8_TO_TCHAR(text)));

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySEditableTextBox_methods[] = {
	{ "select_all_text", (PyCFunction)py_ue_seditable_text_box_select_all_text, METH_VARARGS, "" },
	{ "clear_selection", (PyCFunction)py_ue_seditable_text_box_clear_selection, METH_VARARGS, "" },
	{ "get_selected_text", (PyCFunction)py_ue_seditable_text_box_get_selected_text, METH_VARARGS, "" },
	{ "get_text", (PyCFunction)py_ue_seditable_text_box_get_text, METH_VARARGS, "" },
	{ "set_text", (PyCFunction)py_ue_seditable_text_box_set_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySEditableTextBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SEditableTextBox", /* tp_name */
	sizeof(ue_PySEditableTextBox), /* tp_basicsize */
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
	"Unreal Engine SEditableTextBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySEditableTextBox_methods,             /* tp_methods */
};

static int ue_py_seditable_text_box_init(ue_PySEditableTextBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SEditableTextBox);

	ue_py_slate_farguments_bool("allow_context_menu", AllowContextMenu);
	ue_py_slate_farguments_struct("background_color", BackgroundColor, FSlateColor);
	ue_py_slate_farguments_bool("clear_keyboard_focus_on_commit", ClearKeyboardFocusOnCommit);
	ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_text("hint_text", HintText);
	ue_py_slate_farguments_bool("is_caret_moved_when_gain_focus", IsCaretMovedWhenGainFocus);
	ue_py_slate_farguments_bool("is_password", IsPassword);
	ue_py_slate_farguments_bool("is_read_only", IsReadOnly);
	ue_py_slate_farguments_float("min_desired_width", MinDesiredWidth);
	ue_py_slate_farguments_event("on_context_menu_opening", OnContextMenuOpening, FOnContextMenuOpening, OnContextMenuOpening);
	ue_py_slate_farguments_event("on_key_down_handler", OnKeyDownHandler, FOnKeyDown, OnKeyDown);
	ue_py_slate_farguments_event("on_text_changed", OnTextChanged, FOnTextChanged, OnTextChanged);
	ue_py_slate_farguments_event("on_text_committed", OnTextCommitted, FOnTextCommitted, OnTextCommitted);
	ue_py_slate_farguments_struct("padding", Padding, FMargin);
	ue_py_slate_farguments_struct("read_only_foreground_color", ReadOnlyForegroundColor, FSlateColor);
	ue_py_slate_farguments_bool("revert_text_on_escape", RevertTextOnEscape);
	ue_py_slate_farguments_bool("select_all_text_on_commit", SelectAllTextOnCommit);
	ue_py_slate_farguments_bool("select_all_text_when_focused", SelectAllTextWhenFocused);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FEditableTextBoxStyle);
	ue_py_slate_farguments_text("text", Text);
	ue_py_slate_farguments_optional_enum("text_flow_direction", TextFlowDirection, ETextFlowDirection);
	ue_py_slate_farguments_optional_enum("text_shaping_method", TextShapingMethod, ETextShapingMethod);
	ue_py_slate_farguments_enum("virtual_keyboard_type", VirtualKeyboardType, EKeyboardType);

	ue_py_snew(SEditableTextBox);
	return 0;
}

void ue_python_init_seditable_text_box(PyObject *ue_module)
{

	ue_PySEditableTextBoxType.tp_init = (initproc)ue_py_seditable_text_box_init;

	ue_PySEditableTextBoxType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySEditableTextBoxType) < 0)
		return;

	Py_INCREF(&ue_PySEditableTextBoxType);
	PyModule_AddObject(ue_module, "SEditableTextBox", (PyObject *)&ue_PySEditableTextBoxType);
}

