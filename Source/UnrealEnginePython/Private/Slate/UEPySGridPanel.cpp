
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySGridPanel.h"


#define sw_grid_panel StaticCastSharedRef<SGridPanel>(self->s_panel.s_widget.s_widget)

static PyObject *py_ue_sgrid_panel_clear_children(ue_PySGridPanel *self, PyObject * args) {

	sw_grid_panel->ClearChildren();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_sgrid_panel_add_slot(ue_PySGridPanel *self, PyObject * args) {
	PyObject *py_content;
	int col;
	int row;
	int layer = 0;
	if (!PyArg_ParseTuple(args, "Oii|i:add_slot", &py_content, &col, &row, &layer)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	// TODO: decrement reference when destroying parent
	Py_INCREF(py_swidget);

	SGridPanel::FSlot &fslot = sw_grid_panel->AddSlot(col, row, SGridPanel::Layer(layer));
	fslot.AttachWidget(py_swidget->s_widget->AsShared());

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySGridPanel_methods[] = {
	{ "clear_children", (PyCFunction)py_ue_sgrid_panel_clear_children, METH_VARARGS, "" },
	{ "add_slot", (PyCFunction)py_ue_sgrid_panel_add_slot, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySGridPanelType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SGridPanel", /* tp_name */
	sizeof(ue_PySGridPanel), /* tp_basicsize */
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
	"Unreal Engine SGridPanel",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySGridPanel_methods,             /* tp_methods */
};

static int ue_py_sgrid_panel_init(ue_PySGridPanel *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew_simple(SGridPanel, s_panel.s_widget);
	return 0;
}

void ue_python_init_sgrid_panel(PyObject *ue_module) {

	ue_PySGridPanelType.tp_init = (initproc)ue_py_sgrid_panel_init;
	ue_PySGridPanelType.tp_call = (ternaryfunc)py_ue_sgrid_panel_add_slot;

	ue_PySGridPanelType.tp_base = &ue_PySPanelType;

	if (PyType_Ready(&ue_PySGridPanelType) < 0)
		return;

	Py_INCREF(&ue_PySGridPanelType);
	PyModule_AddObject(ue_module, "SGridPanel", (PyObject *)&ue_PySGridPanelType);
}

