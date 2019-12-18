#include "UEPySCheckBox.h"


static PyObject *py_ue_scheck_box_is_checked(ue_PySCheckBox *self, PyObject * args)
{
	ue_py_slate_cast(SCheckBox);
	if (py_SCheckBox->IsChecked())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject *py_ue_scheck_box_set_is_checked(ue_PySCheckBox *self, PyObject * args)
{
	ue_py_slate_cast(SCheckBox);
	PyObject * py_bool;
	if (!PyArg_ParseTuple(args, "O:set_is_checked", &py_bool))
	{
		return NULL;
	}

	ECheckBoxState CheckedState = PyObject_IsTrue(py_bool) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;

	py_SCheckBox->SetIsChecked(TAttribute<ECheckBoxState>(CheckedState));

	Py_RETURN_NONE;

}

static PyObject *py_ue_scheck_box_set_content(ue_PySCheckBox *self, PyObject * args)
{
	ue_py_slate_cast(SCheckBox);
	PyObject * py_content;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_content))
	{
		return NULL;
	}

	TSharedPtr<SWidget> child = py_ue_is_swidget<SWidget>(py_content);
	if (!child.IsValid())
	{
		return nullptr;
	}



	py_SCheckBox->SetContent(child.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySCheckBox_methods[] = {
	{ "is_checked", (PyCFunction)py_ue_scheck_box_is_checked, METH_VARARGS, "" },
	{ "set_content",      (PyCFunction)py_ue_scheck_box_set_content, METH_VARARGS, "" },
	{ "set_is_checked",      (PyCFunction)py_ue_scheck_box_set_is_checked, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySCheckBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SCheckBox", /* tp_name */
	sizeof(ue_PySCheckBox), /* tp_basicsize */
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
	"Unreal Engine SCheckBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySCheckBox_methods,             /* tp_methods */
};

static int ue_py_scheck_box_init(ue_PySCheckBox *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SCheckBox);
	ue_py_slate_farguments_struct("border_background_color", BorderBackgroundColor, FSlateColor);
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_enum("is_checked", IsChecked, ECheckBoxState);
	ue_py_slate_farguments_event("on_check_state_changed", OnCheckStateChanged, FOnCheckStateChanged, CheckBoxChanged);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FCheckBoxStyle);
	ue_py_slate_farguments_optional_enum("type", Type, ESlateCheckBoxType::Type);
	ue_py_slate_farguments_event("on_check_state_changed", OnCheckStateChanged, FOnCheckStateChanged, CheckBoxChanged);
	ue_py_slate_farguments_enum("is_checked", IsChecked, ECheckBoxState);
	ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
	ue_py_slate_farguments_struct("padding", Padding, FMargin);
	ue_py_slate_farguments_enum("click_method", ClickMethod, EButtonClickMethod::Type);
	ue_py_slate_farguments_optional_bool("is_focusable", IsFocusable);
	ue_py_slate_farguments_optional_struct_ptr("unchecked_image", UncheckedImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("unchecked_hoveredimage", UncheckedHoveredImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("unchecked_pressedimage", UncheckedPressedImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("checked_image", CheckedImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("checked_hoveredimage", CheckedHoveredImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("checked_pressedimage", CheckedPressedImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("undetermined_image", UndeterminedImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("undetermined_hoveredimage", UndeterminedHoveredImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("undetermined_pressedimage", UndeterminedPressedImage, FSlateBrush);
	ue_py_snew(SCheckBox);
	return 0;
}

void ue_python_init_scheck_box(PyObject *ue_module)
{

	ue_PySCheckBoxType.tp_init = (initproc)ue_py_scheck_box_init;
	ue_PySCheckBoxType.tp_call = (ternaryfunc)py_ue_scheck_box_set_content;

	ue_PySCheckBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySCheckBoxType) < 0)
		return;

	Py_INCREF(&ue_PySCheckBoxType);
	PyModule_AddObject(ue_module, "SCheckBox", (PyObject *)&ue_PySCheckBoxType);
}
