
#include "UEPySVectorInputBox.h"


static PyMethodDef ue_PySVectorInputBox_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySVectorInputBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SVectorInputBox", /* tp_name */
	sizeof(ue_PySVectorInputBox), /* tp_basicsize */
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
	"Unreal Engine SVectorInputBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySVectorInputBox_methods,             /* tp_methods */
};

static int ue_py_svector_input_box_init(ue_PySVectorInputBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SVectorInputBox);

#if ENGINE_MINOR_VERSION > 15
	ue_py_slate_farguments_optional_bool("allow_spin", AllowSpin);
#endif
	ue_py_slate_farguments_optional_bool("allow_responsive_layout", AllowResponsiveLayout);
	ue_py_slate_farguments_optional_bool("color_axis_labels", bColorAxisLabels);
	ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
	ue_py_slate_farguments_event("on_x_changed", OnXChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_y_changed", OnYChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_z_changed", OnZChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_x_committed", OnXCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_event("on_y_committed", OnYCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_event("on_z_committed", OnZCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_tfloat("x", X);
	ue_py_slate_farguments_tfloat("y", Y);
	ue_py_slate_farguments_tfloat("z", Z);

	ue_py_snew(SVectorInputBox);
	return 0;
}

void ue_python_init_svector_input_box(PyObject *ue_module)
{

	ue_PySVectorInputBoxType.tp_init = (initproc)ue_py_svector_input_box_init;

	ue_PySVectorInputBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySVectorInputBoxType) < 0)
		return;

	Py_INCREF(&ue_PySVectorInputBoxType);
	PyModule_AddObject(ue_module, "SVectorInputBox", (PyObject *)&ue_PySVectorInputBoxType);
}
