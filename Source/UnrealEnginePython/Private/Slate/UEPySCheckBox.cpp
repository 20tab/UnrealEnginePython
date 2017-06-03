
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySCheckBox.h"


#define sw_check_box StaticCastSharedRef<SCheckBox>(self->s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_scheck_box_is_checked(ue_PySCheckBox *self, PyObject * args) {

	if (sw_check_box->IsChecked()) {
		Py_RETURN_TRUE;
	}
	
	Py_RETURN_FALSE;
}

static PyMethodDef ue_PySCheckBox_methods[] = {
	{ "is_checked", (PyCFunction)py_ue_scheck_box_is_checked, METH_VARARGS, "" },
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

static int ue_py_scheck_box_init(ue_PySCheckBox *self, PyObject *args, PyObject *kwargs) {
	
	ue_py_slate_setup_farguments(SCheckBox);
	ue_py_slate_farguments_struct("border_background_color", BorderBackgroundColor, FSlateColor);
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_enum("is_checked", IsChecked, ECheckBoxState);
	ue_py_slate_farguments_event("on_check_state_changed", OnCheckStateChanged, FOnCheckStateChanged, CheckBoxChanged);

	ue_py_snew(SCheckBox, s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_scheck_box(PyObject *ue_module) {

	ue_PySCheckBoxType.tp_init = (initproc)ue_py_scheck_box_init;

	ue_PySCheckBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySCheckBoxType) < 0)
		return;

	Py_INCREF(&ue_PySCheckBoxType);
	PyModule_AddObject(ue_module, "SCheckBox", (PyObject *)&ue_PySCheckBoxType);
}
