
#include "UEPySScrollBox.h"


static PyObject *py_ue_sscroll_box_add_slot(ue_PySScrollBox *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(SScrollBox);

	int32 retCode = [&]() {
		ue_py_slate_setup_hack_slot_args(SScrollBox, py_SScrollBox);
		ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
		ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);
		return 0;
	}();

	if (retCode != 0)
	{
		return PyErr_Format(PyExc_Exception, "could not add vertical slot");
	}

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_sscroll_box_clear_children(ue_PySScrollBox *self, PyObject * args)
{
	ue_py_slate_cast(SScrollBox);
	py_SScrollBox->ClearChildren();

	Py_RETURN_NONE;
}

static PyMethodDef ue_PySScrollBox_methods[] = {
	{ "clear_children", (PyCFunction)py_ue_sscroll_box_clear_children, METH_VARARGS, "" },
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_sscroll_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySScrollBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SScrollBox", /* tp_name */
	sizeof(ue_PySScrollBox), /* tp_basicsize */
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
	"Unreal Engine SScrollBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySScrollBox_methods,             /* tp_methods */
};

static int ue_py_sscroll_box_init(ue_PySScrollBox *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SScrollBox);

	ue_py_slate_farguments_optional_enum("allow_overscroll", AllowOverscroll, EAllowOverscroll);
	ue_py_slate_farguments_optional_enum("consume_mouse_wheel", ConsumeMouseWheel, EConsumeMouseWheel);
	ue_py_slate_farguments_optional_enum("orientation", Orientation, EOrientation);
	ue_py_slate_farguments_optional_bool("scroll_bar_always_visible", ScrollBarAlwaysVisible);
	ue_py_slate_farguments_optional_struct_ptr("scroll_bar_style", ScrollBarStyle, FScrollBarStyle);
	ue_py_slate_farguments_optional_fvector2d("scroll_bar_thickness", ScrollBarThickness);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FScrollBoxStyle);

	ue_py_snew(SScrollBox);

	return 0;
}

void ue_python_init_sscroll_box(PyObject *ue_module)
{

	ue_PySScrollBoxType.tp_init = (initproc)ue_py_sscroll_box_init;
	ue_PySScrollBoxType.tp_call = (ternaryfunc)py_ue_sscroll_box_add_slot;

	ue_PySScrollBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySScrollBoxType) < 0)
		return;

	Py_INCREF(&ue_PySScrollBoxType);
	PyModule_AddObject(ue_module, "SScrollBox", (PyObject *)&ue_PySScrollBoxType);
}
