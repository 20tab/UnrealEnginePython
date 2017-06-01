
#include "UnrealEnginePythonPrivatePCH.h"


#include "UEPySDockTab.h"

#define sw_dock_tab StaticCastSharedRef<SDockTab>(self->s_border.s_compound_widget.s_widget.s_widget)


static PyObject *py_ue_sdock_tab_set_label(ue_PySButton *self, PyObject * args) {
	char *label;
	if (!PyArg_ParseTuple(args, "s:set_label", &label)) {
		return NULL;
	}

	sw_dock_tab->SetLabel(FText::FromString(UTF8_TO_TCHAR(label)));

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySDockTab_methods[] = {
	{ "set_label", (PyCFunction)py_ue_sdock_tab_set_label, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySDockTabType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SDockTab", /* tp_name */
	sizeof(ue_PySDockTab), /* tp_basicsize */
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
	"Unreal Engine SDockTab",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySDockTab_methods,             /* tp_methods */
};

void ue_python_init_sdock_tab(PyObject *ue_module) {

	ue_PySDockTabType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySDockTabType) < 0)
		return;

	Py_INCREF(&ue_PySDockTabType);
	PyModule_AddObject(ue_module, "SDockTab", (PyObject *)&ue_PySDockTabType);
}
