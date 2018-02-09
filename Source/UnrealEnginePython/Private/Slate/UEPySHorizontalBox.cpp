
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySHorizontalBox.h"


#define sw_horizontal_box StaticCastSharedRef<SHorizontalBox>(self->s_box_panel.s_panel.s_widget.s_widget)


static PyObject *py_ue_shorizontal_box_add_slot(ue_PySHorizontalBox *self, PyObject * args, PyObject *kwargs)
{
    int32 retCode = [&]() {
        ue_py_slate_setup_hack_slot_args(SHorizontalBox, sw_horizontal_box);
        ue_py_slate_farguments_float("fill_width", FillWidth);
        ue_py_slate_farguments_float("max_width", MaxWidth);
        ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
        ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);

        //NOTE: Padding slot in slate is weird and manually supports different parameter constructions
	    if (PyObject *padding = ue_py_dict_get_item(kwargs, "padding"))
	    {
		    if (PyTuple_Check(padding))
		    {
			    FMargin margin;
			    if (!PyArg_ParseTuple(padding, "f|fff", &margin.Left, &margin.Top, &margin.Right, &margin.Bottom))
			    {
                    PyErr_SetString(PyExc_TypeError, "invalid padding value");
                    return -1;
			    }
			    arguments.Padding(margin);
		    }
		    else if (PyNumber_Check(padding))
		    {
			    PyObject *py_float = PyNumber_Float(padding);
			    arguments.Padding(PyFloat_AsDouble(py_float));
			    Py_DECREF(py_float);
		    }
		    else
		    {
                ue_py_slate_farguments_struct("padding", Padding, FMargin);
		    }
	    }
        PyObject *py_auto_width = ue_py_dict_get_item(kwargs, "auto_width");
	    if (py_auto_width && PyObject_IsTrue(py_auto_width))
        { arguments.AutoWidth(); }

        return 0;
    }();

    if (retCode != 0)
    {
        return PyErr_Format(PyExc_Exception, "could not add horizontal slot");
    }

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_shorizontal_box_num_slots(ue_PySHorizontalBox *self, PyObject * args)
{
	return PyLong_FromLong(sw_horizontal_box->NumSlots());
}

static PyMethodDef ue_PySHorizontalBox_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_shorizontal_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "num_slots", (PyCFunction)py_ue_shorizontal_box_num_slots, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySHorizontalBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SHorizontalBox", /* tp_name */
	sizeof(ue_PySHorizontalBox), /* tp_basicsize */
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
	"Unreal Engine SHorizontalBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySHorizontalBox_methods,             /* tp_methods */
};

static int ue_py_shorizontal_box_init(ue_PySHorizontalBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SHorizontalBox, s_box_panel.s_panel.s_widget);
	return 0;
}


void ue_python_init_shorizontal_box(PyObject *ue_module)
{

	ue_PySHorizontalBoxType.tp_init = (initproc)ue_py_shorizontal_box_init;
	ue_PySHorizontalBoxType.tp_call = (ternaryfunc)py_ue_shorizontal_box_add_slot;

	ue_PySHorizontalBoxType.tp_base = &ue_PySBoxPanelType;

	if (PyType_Ready(&ue_PySHorizontalBoxType) < 0)
		return;

	Py_INCREF(&ue_PySHorizontalBoxType);
	PyModule_AddObject(ue_module, "SHorizontalBox", (PyObject *)&ue_PySHorizontalBoxType);
}
