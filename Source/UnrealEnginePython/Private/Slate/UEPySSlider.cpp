
#include "UEPySSlider.h"

static PyObject *py_ue_sslider_set_value(ue_PySSlider *self, PyObject * args)
{
	ue_py_slate_cast(SSlider);
	float value;
	if (!PyArg_ParseTuple(args, "f:set_value", &value))
	{
		return nullptr;
	}

	py_SSlider->SetValue(value);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_sslider_get_value(ue_PySSlider *self, PyObject * args)
{
	ue_py_slate_cast(SSlider);
	return PyFloat_FromDouble(py_SSlider->GetValue());
}

static PyMethodDef ue_PySSlider_methods[] = {
	{ "set_value", (PyCFunction)py_ue_sslider_set_value, METH_VARARGS, "" },
	{ "get_value", (PyCFunction)py_ue_sslider_get_value, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySSliderType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SSlider", /* tp_name */
	sizeof(ue_PySSlider), /* tp_basicsize */
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
	"Unreal Engine SSlider",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySSlider_methods,             /* tp_methods */
};

static int ue_py_sslider_init(ue_PySSlider *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SSlider);

	ue_py_slate_farguments_bool("indent_handle", IndentHandle);
	ue_py_slate_farguments_optional_bool("is_focusable", IsFocusable);
	ue_py_slate_farguments_bool("locked", Locked);
	ue_py_slate_farguments_event("on_controller_capture_begin", OnControllerCaptureBegin, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_controller_capture_end", OnControllerCaptureEnd, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_mouse_capture_begin", OnMouseCaptureBegin, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_mouse_capture_end", OnMouseCaptureEnd, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_value_changed", OnValueChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_optional_enum("orientation", Orientation, EOrientation);
	ue_py_slate_farguments_struct("slider_bar_color", SliderBarColor, FSlateColor);
	ue_py_slate_farguments_struct("slider_handle_color", SliderHandleColor, FSlateColor);
	ue_py_slate_farguments_float("step_size", StepSize);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FSliderStyle);
	ue_py_slate_farguments_float("value", Value);

	ue_py_snew(SSlider);
	return 0;
}

void ue_python_init_sslider(PyObject *ue_module)
{
	ue_PySSliderType.tp_init = (initproc)ue_py_sslider_init;

	ue_PySSliderType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySSliderType) < 0)
		return;

	Py_INCREF(&ue_PySSliderType);
	PyModule_AddObject(ue_module, "SSlider", (PyObject *)&ue_PySSliderType);
}
