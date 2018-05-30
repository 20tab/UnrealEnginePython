

#include "UEPySNumericEntryBox.h"


static PyMethodDef ue_PySNumericEntryBox_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySNumericEntryBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SNumericEntryBox", /* tp_name */
	sizeof(ue_PySNumericEntryBox), /* tp_basicsize */
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
	"Unreal Engine SNumericEntryBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySNumericEntryBox_methods,             /* tp_methods */
};

static int ue_py_snumeric_entry_box_init(ue_PySNumericEntryBox *self, PyObject *args, PyObject *kwargs)
{

	PyObject *py_numeric_type = ue_py_dict_get_item(kwargs, "numeric_type");
	if (py_numeric_type && PyLong_Check(py_numeric_type))
	{
		ue_py_slate_setup_farguments(SNumericEntryBox<int32>);

		ue_py_slate_farguments_optional_bool("allow_spin", AllowSpin);
		ue_py_slate_farguments_optional_struct("border_background_color", BorderBackgroundColor, FSlateColor);
		ue_py_slate_farguments_optional_struct("border_foreground_color", BorderForegroundColor, FSlateColor);
		ue_py_slate_farguments_int("delta", Delta);
		ue_py_slate_farguments_optional_struct_ptr("editable_text_box_style", EditableTextBoxStyle, FEditableTextBoxStyle);
		ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
		ue_py_slate_farguments_optional_struct("label_padding", LabelPadding, FMargin);
		ue_py_slate_farguments_optional_enum("label_v_align", LabelVAlign, EVerticalAlignment);
		ue_py_slate_farguments_tint("max_slider_value", MaxSliderValue);
		ue_py_slate_farguments_tint("max_value", MaxValue);
		ue_py_slate_farguments_float("min_desired_value_width", MinDesiredValueWidth);
		ue_py_slate_farguments_tint("min_slider_value", MinSliderValue);
		ue_py_slate_farguments_tint("min_value", MinValue);
		ue_py_slate_farguments_struct("override_text_margin", OverrideTextMargin, FMargin);
		ue_py_slate_farguments_float("slider_exponent", SliderExponent);
		ue_py_slate_farguments_optional_text("undetermined_string", UndeterminedString);
		ue_py_slate_farguments_tint("value", Value);
		ue_py_slate_farguments_event("on_begin_slider_movement", OnBeginSliderMovement, FSimpleDelegate, SimpleExecuteAction);
		ue_py_slate_farguments_event("on_end_slider_movement", OnEndSliderMovement, FOnInt32ValueChanged, OnInt32Changed);
		ue_py_slate_farguments_event("on_value_changed", OnValueChanged, FOnInt32ValueChanged, OnInt32Changed);
		ue_py_slate_farguments_event("on_value_committed", OnValueCommitted, FOnInt32ValueCommitted, OnInt32Committed);

		ue_py_snew(SNumericEntryBox<int32>);
	}
	else
	{
		ue_py_slate_setup_farguments(SNumericEntryBox<float>);

		ue_py_slate_farguments_optional_bool("allow_spin", AllowSpin);
		ue_py_slate_farguments_optional_struct("border_background_color", BorderBackgroundColor, FSlateColor);
		ue_py_slate_farguments_optional_struct("border_foreground_color", BorderForegroundColor, FSlateColor);
		ue_py_slate_farguments_float("delta", Delta);
		ue_py_slate_farguments_optional_struct_ptr("editable_text_box_style", EditableTextBoxStyle, FEditableTextBoxStyle);
		ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
		ue_py_slate_farguments_optional_struct("label_padding", LabelPadding, FMargin);
		ue_py_slate_farguments_optional_enum("label_v_align", LabelVAlign, EVerticalAlignment);
		ue_py_slate_farguments_tfloat("max_slider_value", MaxSliderValue);
		ue_py_slate_farguments_tfloat("max_value", MaxValue);
		ue_py_slate_farguments_float("min_desired_value_width", MinDesiredValueWidth);
		ue_py_slate_farguments_tfloat("min_slider_value", MinSliderValue);
		ue_py_slate_farguments_tfloat("min_value", MinValue);
		ue_py_slate_farguments_struct("override_text_margin", OverrideTextMargin, FMargin);
		ue_py_slate_farguments_float("slider_exponent", SliderExponent);
		ue_py_slate_farguments_optional_text("undetermined_string", UndeterminedString);
		ue_py_slate_farguments_tfloat("value", Value);
		ue_py_slate_farguments_event("on_begin_slider_movement", OnBeginSliderMovement, FSimpleDelegate, SimpleExecuteAction);
		ue_py_slate_farguments_event("on_end_slider_movement", OnEndSliderMovement, FOnFloatValueChanged, OnFloatChanged);
		ue_py_slate_farguments_event("on_value_changed", OnValueChanged, FOnFloatValueChanged, OnFloatChanged);
		ue_py_slate_farguments_event("on_value_committed", OnValueCommitted, FOnFloatValueCommitted, OnFloatCommitted);

		ue_py_snew(SNumericEntryBox<float>);
	}

	return 0;
}

void ue_python_init_snumeric_entry_box(PyObject *ue_module)
{

	ue_PySNumericEntryBoxType.tp_init = (initproc)ue_py_snumeric_entry_box_init;

	ue_PySNumericEntryBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySNumericEntryBoxType) < 0)
		return;

	Py_INCREF(&ue_PySNumericEntryBoxType);
	PyModule_AddObject(ue_module, "SNumericEntryBox", (PyObject *)&ue_PySNumericEntryBoxType);
}
