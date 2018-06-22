#include "UEPySProgressBar.h"

static PyObject *py_ue_sprogress_bar_set_percent(ue_PySProgressBar *self, PyObject *args)
{
	ue_py_slate_cast(SProgressBar);
	float percent;
	if (!PyArg_ParseTuple(args, "f", &percent))
	{
		return nullptr;
	}
	py_SProgressBar->SetPercent(percent);

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySProgressBar_methods[] = {
	{ "set_percent", (PyCFunction)py_ue_sprogress_bar_set_percent, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySProgressBarType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SProgressBar", /* tp_name */
	sizeof(ue_PySProgressBar), /* tp_basicsize */
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
	"Unreal Engine SProgressBar",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySProgressBar_methods,             /* tp_methods */
};

static int ue_py_sprogress_bar_init(ue_PySProgressBar *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SProgressBar);

	ue_py_slate_farguments_optional_struct_ptr("background_image", BackgroundImage, FSlateBrush);
	ue_py_slate_farguments_optional_enum("bar_fill_type", BarFillType, EProgressBarFillType::Type);
	ue_py_slate_farguments_fvector2d("border_padding", BorderPadding);
	ue_py_slate_farguments_struct("fill_color_and_opacity", FillColorAndOpacity, FSlateColor);
	ue_py_slate_farguments_optional_struct_ptr("fill_image", FillImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("marquee_image", MarqueeImage, FSlateBrush);
	ue_py_slate_farguments_tfloat("percent", Percent);
	ue_py_slate_farguments_optional_float("refresh_rate", RefreshRate);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FProgressBarStyle);

	ue_py_snew(SProgressBar);
	return 0;
}

void ue_python_init_sprogress_bar(PyObject *ue_module)
{

	ue_PySProgressBarType.tp_init = (initproc)ue_py_sprogress_bar_init;

	ue_PySProgressBarType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySProgressBarType) < 0)
		return;

	Py_INCREF(&ue_PySProgressBarType);
	PyModule_AddObject(ue_module, "SProgressBar", (PyObject *)&ue_PySProgressBarType);
}
