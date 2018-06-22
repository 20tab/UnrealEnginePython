

#include "UEPySRotatorInputBox.h"


static PyMethodDef ue_PySRotatorInputBox_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySRotatorInputBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SRotatorInputBox", /* tp_name */
	sizeof(ue_PySRotatorInputBox), /* tp_basicsize */
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
	"Unreal Engine SRotatorInputBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySRotatorInputBox_methods,             /* tp_methods */
};

static int ue_py_srotator_input_box_init(ue_PySRotatorInputBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SRotatorInputBox);

	ue_py_slate_farguments_optional_bool("allow_responsive_layout", AllowResponsiveLayout);
	ue_py_slate_farguments_optional_bool("color_axis_labels", bColorAxisLabels);
	ue_py_slate_farguments_optional_bool("allow_spin", AllowSpin);
	ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
	ue_py_slate_farguments_event("on_roll_changed", OnRollChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_pitch_changed", OnPitchChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_yaw_changed", OnYawChanged, FOnFloatValueChanged, OnFloatChanged);
	ue_py_slate_farguments_event("on_roll_committed", OnRollCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_event("on_pitch_committed", OnPitchCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_event("on_yaw_committed", OnYawCommitted, FOnFloatValueCommitted, OnFloatCommitted);
	ue_py_slate_farguments_tfloat("roll", Roll);
	ue_py_slate_farguments_tfloat("pitch", Pitch);
	ue_py_slate_farguments_tfloat("yaw", Yaw);

	ue_py_snew(SRotatorInputBox);
	return 0;
}

void ue_python_init_srotator_input_box(PyObject *ue_module)
{

	ue_PySRotatorInputBoxType.tp_init = (initproc)ue_py_srotator_input_box_init;

	ue_PySRotatorInputBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySRotatorInputBoxType) < 0)
		return;

	Py_INCREF(&ue_PySRotatorInputBoxType);
	PyModule_AddObject(ue_module, "SRotatorInputBox", (PyObject *)&ue_PySRotatorInputBoxType);
}
