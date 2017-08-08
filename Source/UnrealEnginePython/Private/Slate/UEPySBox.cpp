
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySBox.h"


#define sw_box StaticCastSharedRef<SBox>(self->s_panel.s_widget.s_widget)

static PyObject *py_ue_sbox_set_content(ue_PySBox *self, PyObject * args) {
	PyObject *py_content;
	if (!PyArg_ParseTuple(args, "O:set_content", &py_content)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}

	Py_XDECREF(self->s_panel.s_widget.py_swidget_content);
	Py_INCREF(py_swidget);
	self->s_panel.s_widget.py_swidget_content = py_swidget;

	sw_box->SetContent(py_swidget->s_widget->AsShared());

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySBox_methods[] = {
	{ "set_content", (PyCFunction)py_ue_sbox_set_content, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SBox", /* tp_name */
	sizeof(ue_PySBox), /* tp_basicsize */
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
	"Unreal Engine SBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySBox_methods,             /* tp_methods */
};

static int ue_py_sbox_init(ue_PySBox *self, PyObject *args, PyObject *kwargs) {

	ue_py_slate_setup_farguments(SBox);

	ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
	ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);
	ue_py_slate_farguments_struct("padding", Padding, FMargin);
	ue_py_slate_farguments_optional_foptional_size("height_override", HeightOverride);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_foptional_size("max_aspect_ratio", MaxAspectRatio);
#endif
	ue_py_slate_farguments_optional_foptional_size("max_desired_height", MaxDesiredHeight);
	ue_py_slate_farguments_optional_foptional_size("max_desired_width", MaxDesiredWidth);
	ue_py_slate_farguments_optional_foptional_size("min_desired_height", MinDesiredHeight);
	ue_py_slate_farguments_optional_foptional_size("min_desired_width", MinDesiredWidth);
	
	ue_py_snew(SBox, s_panel.s_widget);

	return 0;
}

void ue_python_init_sbox(PyObject *ue_module) {

	ue_PySBoxType.tp_init = (initproc)ue_py_sbox_init;
	ue_PySBoxType.tp_call = (ternaryfunc)py_ue_sbox_set_content;
	ue_PySBoxType.tp_base = &ue_PySPanelType;

	if (PyType_Ready(&ue_PySBoxType) < 0)
		return;

	Py_INCREF(&ue_PySBoxType);
	PyModule_AddObject(ue_module, "SBox", (PyObject *)&ue_PySBoxType);
}
