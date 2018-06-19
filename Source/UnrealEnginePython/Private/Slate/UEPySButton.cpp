

#include "UEPySButton.h"

static PyObject *py_ue_sbutton_is_pressed(ue_PySButton *self, PyObject * args)
{
	ue_py_slate_cast(SButton);

	if (py_SButton->IsPressed())
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}


static PyObject *py_ue_sbutton_bind_on_clicked(ue_PySButton *self, PyObject * args)
{
	ue_py_slate_cast(SButton);
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_clicked", &py_callable))
	{
		return nullptr;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FOnClicked handler;
	TSharedRef<FPythonSlateDelegate>py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewSlateDelegate(self->s_border.s_compound_widget.s_widget.Widget, py_callable);
	py_delegate->SetPyCallable(py_callable);
	handler.BindSP(py_delegate, &FPythonSlateDelegate::OnClicked);

	py_SButton->SetOnClicked(handler);

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySButton_methods[] = {
	{ "is_pressed", (PyCFunction)py_ue_sbutton_is_pressed, METH_VARARGS, "" },
	{ "bind_on_clicked", (PyCFunction)py_ue_sbutton_bind_on_clicked, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySButtonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SButton", /* tp_name */
	sizeof(ue_PySButton), /* tp_basicsize */
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
	"Unreal Engine SButton",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySButton_methods,             /* tp_methods */
};

static int ue_py_sbutton_init(ue_PySButton *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SButton);

	ue_py_slate_farguments_struct("button_color_and_opacity", ButtonColorAndOpacity, FSlateColor);
	ue_py_slate_farguments_optional_enum("click_method", ClickMethod, EButtonClickMethod::Type);
	ue_py_slate_farguments_struct("content_padding", ContentPadding, FMargin);
	ue_py_slate_farguments_fvector2d("content_scale", ContentScale);
	ue_py_slate_farguments_fvector2d("desired_size_scale", DesiredSizeScale);
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
	ue_py_slate_farguments_optional_struct("hovered_sound_override", HoveredSoundOverride, FSlateSound);
	ue_py_slate_farguments_optional_bool("is_focusable", IsFocusable);
	ue_py_slate_farguments_event("on_clicked", OnClicked, FOnClicked, OnClicked);
	ue_py_slate_farguments_event("on_hovered", OnHovered, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_pressed", OnPressed, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_released", OnReleased, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_event("on_unhovered", OnUnhovered, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_optional_struct("pressed_sound_override", PressedSoundOverride, FSlateSound);
	ue_py_slate_farguments_optional_enum("press_method", PressMethod, EButtonPressMethod::Type);
	ue_py_slate_farguments_text("text", Text);
	ue_py_slate_farguments_optional_enum("text_flow_direction", TextFlowDirection, ETextFlowDirection);
	ue_py_slate_farguments_optional_enum("text_shaping_method", TextShapingMethod, ETextShapingMethod);
	ue_py_slate_farguments_optional_struct_ptr("button_style", ButtonStyle, FButtonStyle);
	ue_py_slate_farguments_optional_struct_ptr("text_style", TextStyle, FTextBlockStyle);
	ue_py_slate_farguments_optional_enum("touch_method", TouchMethod, EButtonTouchMethod::Type);
	ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);

	ue_py_snew(SButton);
	return 0;
}

void ue_python_init_sbutton(PyObject *ue_module)
{

	ue_PySButtonType.tp_init = (initproc)ue_py_sbutton_init;

	ue_PySButtonType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySButtonType) < 0)
		return;

	Py_INCREF(&ue_PySButtonType);
	PyModule_AddObject(ue_module, "SButton", (PyObject *)&ue_PySButtonType);
}
