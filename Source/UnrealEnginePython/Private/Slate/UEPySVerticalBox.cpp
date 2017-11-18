
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySVerticalBox.h"


#define sw_vertical_box StaticCastSharedRef<SVerticalBox>(self->s_box_panel.s_panel.s_widget.s_widget)

static PyObject *py_ue_svertical_box_add_slot(ue_PySVerticalBox *self, PyObject * args, PyObject *kwargs) {
	PyObject *py_content;
	int h_align = 0;
	float max_height = 0;
	PyObject *padding = nullptr;
	int v_align = 0;
	float fill_height = 0;
	PyObject *py_auto_height = nullptr;

	char *kwlist[] = { (char *)"widget",
		(char *)"h_align",
		(char *)"max_height",
		(char *)"padding",
		(char *)"v_align",
		(char *)"fill_height",
		(char *)"auto_height",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ifOifO:add_slot", kwlist,
		&py_content,
		&h_align,
		&max_height,
		&padding,
		&v_align,
		&fill_height,
		&py_auto_height)) {
		return NULL;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget) {
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}
	
	Py_INCREF(py_swidget);
	self->s_box_panel.s_panel.s_widget.py_swidget_slots.Add(py_swidget);

	SVerticalBox::FSlot &fslot = sw_vertical_box->AddSlot();
	fslot.AttachWidget(py_swidget->s_widget->AsShared());
	fslot.HAlign((EHorizontalAlignment)h_align);
	if (max_height != 0)
		fslot.MaxHeight(max_height);
	if (fill_height)
		fslot.FillHeight(fill_height);
	if (padding)
	{
		if (PyTuple_Check(padding))
		{
			FMargin margin;
			if (!PyArg_ParseTuple(padding, "f|fff", &margin.Left, &margin.Top, &margin.Right, &margin.Bottom))
			{
				return PyErr_Format(PyExc_Exception, "invalid padding value");
			}
			fslot.Padding(margin);
		}
		else if (PyNumber_Check(padding))
		{
			PyObject *py_float = PyNumber_Float(padding);
			fslot.Padding(PyFloat_AsDouble(py_float));
			Py_DECREF(py_float);
		}
		else
		{
			return PyErr_Format(PyExc_Exception, "invalid padding value");
		}
	}
	fslot.VAlign((EVerticalAlignment)v_align);
	if (py_auto_height && PyObject_IsTrue(py_auto_height))
		fslot.AutoHeight();

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_svertical_box_num_slots(ue_PySHorizontalBox *self, PyObject * args) {
	return PyLong_FromLong(sw_vertical_box->NumSlots());
}

static PyMethodDef ue_PySVerticalBox_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_svertical_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "num_slots", (PyCFunction)py_ue_svertical_box_num_slots, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySVerticalBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SVerticalBox", /* tp_name */
	sizeof(ue_PySVerticalBox), /* tp_basicsize */
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
	"Unreal Engine SVerticalBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySVerticalBox_methods,             /* tp_methods */
};

static int ue_py_svertical_box_init(ue_PySHorizontalBox *self, PyObject *args, PyObject *kwargs) {
	
	ue_py_slate_setup_farguments(SVerticalBox);

	ue_py_snew(SVerticalBox, s_box_panel.s_panel.s_widget);
	return 0;
}


void ue_python_init_svertical_box(PyObject *ue_module) {

	ue_PySVerticalBoxType.tp_init = (initproc)ue_py_svertical_box_init;
	ue_PySVerticalBoxType.tp_call = (ternaryfunc)py_ue_svertical_box_add_slot;

	ue_PySVerticalBoxType.tp_base = &ue_PySBoxPanelType;

	if (PyType_Ready(&ue_PySVerticalBoxType) < 0)
		return;

	Py_INCREF(&ue_PySVerticalBoxType);
	PyModule_AddObject(ue_module, "SVerticalBox", (PyObject *)&ue_PySVerticalBoxType);
}
