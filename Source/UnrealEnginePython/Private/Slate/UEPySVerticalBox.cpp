
#include "UEPySVerticalBox.h"


static PyObject *py_ue_svertical_box_add_slot(ue_PySVerticalBox *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(SVerticalBox);

	int32 retCode = [&]() {
		ue_py_slate_setup_hack_slot_args(SVerticalBox, py_SVerticalBox);
		ue_py_slate_farguments_float("fill_height", FillHeight);
		ue_py_slate_farguments_float("max_height", MaxHeight);
		ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
		ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);
		ue_py_slate_farguments_call("auto_height", AutoHeight);
		ue_py_slate_farguments_padding("padding", Padding);
		return 0;
	}();

	if (retCode != 0)
	{
		return PyErr_Format(PyExc_Exception, "could not add vertical slot");
	}

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_svertical_box_num_slots(ue_PySVerticalBox *self, PyObject * args)
{
	ue_py_slate_cast(SVerticalBox);
	return PyLong_FromLong(py_SVerticalBox->NumSlots());
}

static PyMethodDef ue_PySVerticalBox_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_svertical_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "num_slots", (PyCFunction)py_ue_svertical_box_num_slots, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(SVerticalBox);

static int ue_py_svertical_box_init(ue_PySVerticalBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SVerticalBox);
	return 0;
}

PyNumberMethods ue_PySVerticalBox_number_methods;

static PyObject *ue_py_svertical_box_add(ue_PySVerticalBox *self, PyObject *value)
{
	ue_py_slate_cast(SVerticalBox);

	TSharedPtr<SWidget> Child = py_ue_is_swidget<SWidget>(value);
	if (!Child.IsValid())
	{
		return nullptr;
	}

	SVerticalBox::FSlot &fslot = py_SVerticalBox->AddSlot();
	fslot.AttachWidget(Child.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}

void ue_python_init_svertical_box(PyObject *ue_module)
{

	ue_PySVerticalBoxType.tp_init = (initproc)ue_py_svertical_box_init;
	ue_PySVerticalBoxType.tp_call = (ternaryfunc)py_ue_svertical_box_add_slot;
	memset(&ue_PySVerticalBox_number_methods, 0, sizeof(PyNumberMethods));
	ue_PySVerticalBoxType.tp_as_number = &ue_PySVerticalBox_number_methods;
	ue_PySVerticalBox_number_methods.nb_add = (binaryfunc)ue_py_svertical_box_add;

	ue_PySVerticalBoxType.tp_base = &ue_PySBoxPanelType;

	if (PyType_Ready(&ue_PySVerticalBoxType) < 0)
		return;

	Py_INCREF(&ue_PySVerticalBoxType);
	PyModule_AddObject(ue_module, "SVerticalBox", (PyObject *)&ue_PySVerticalBoxType);
}
