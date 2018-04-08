#include "UEPySGraphPanel.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 15


#define sw_graph_panel StaticCastSharedRef<SGraphPanel>(self->s_nodePanel.s_panel.s_widget)

/*
static PyObject *py_ue_sgraph_panel_add_slot(ue_PySGraphPanel* self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_content;
	int z_order = -1;
	int h_align = 0;
	PyObject *padding = nullptr;
	int v_align = 0;

	char *kwlist[] = { (char *)"widget",
		(char *)"z_order",
		(char *)"h_align",
		(char *)"padding",
		(char *)"v_align",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|iiOi:add_slot", kwlist,
		&py_content,
		&z_order,
		&h_align,
		&padding,
		&v_align))
	{
		return nullptr;
	}

	ue_PySWidget *py_swidget = py_ue_is_swidget(py_content);
	if (!py_swidget)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");
	}

	Py_INCREF(py_swidget);
	self->s_nodePanel.s_panel.s_widget.py_swidget_slots.Add(py_swidget);

	sw_graph_panel;

	SOverlay::FOverlaySlot &fslot = sw_graph_panel->//sw_overlay->AddSlot(z_order);
	fslot.AttachWidget(py_swidget->s_widget->AsShared());
	fslot.HAlign((EHorizontalAlignment)h_align);
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

	Py_INCREF(self);
	return (PyObject *)self;
}
*/

static PyMethodDef ue_PySGraphPanel_methods[] = {
	//{"add_slot", (PyCFunction)py_ue_sgraph_panel_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySGraphPanelType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SGraphPanel", /* tp_name */
	sizeof(ue_PySGraphPanel), /* tp_basicsize */
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
	"Unreal Engine SGraphPanel",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySGraphPanel_methods,             /* tp_methods */
};

//why do you need to return an integer?
static int ue_py_sgraph_panel_init(ue_PySGraphPanel *self, PyObject *args, PyObject *kwargs)
{
	//so for right now, let's just have this commented out to see if we get any errors
	//if says we don't have s_nodePanel
	ue_py_snew_simple(SGraphPanel);//s_nodePanel.s_panel);
	//ue_py_snew(SGraphPanel, s_nodePanel.s_panel.s_widget);

	return 0;
}

void ue_python_init_sgraph_panel(PyObject *ue_module)
{
	//ue_PySGraphPanelType.tp_init = (initproc)ue_py_sgraph_panel_init;
	//ue_PySGraphPanelType.tp_call = (ternaryfunc)py_ue_sgraph_panel_add_slot;
	ue_PySGraphPanelType.tp_base = &ue_PySNodePanelType;

	if (PyType_Ready(&ue_PySGraphPanelType) < 0)
		return;

	Py_INCREF(&ue_PySGraphPanelType);
	PyModule_AddObject(ue_module, "SGraphPanel", (PyObject *)&ue_PySGraphPanelType);
}

#endif
#endif