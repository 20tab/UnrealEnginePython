
#include "UEPySHorizontalBox.h"


static PyObject *py_ue_shorizontal_box_add_slot(ue_PySHorizontalBox *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(SHorizontalBox);

	int32 retCode = [&]() {
		ue_py_slate_setup_hack_slot_args(SHorizontalBox, py_SHorizontalBox);
		ue_py_slate_farguments_float("fill_width", FillWidth);
		ue_py_slate_farguments_float("max_width", MaxWidth);
		ue_py_slate_farguments_optional_enum("h_align", HAlign, EHorizontalAlignment);
		ue_py_slate_farguments_optional_enum("v_align", VAlign, EVerticalAlignment);
		ue_py_slate_farguments_call("auto_width", AutoWidth);
		ue_py_slate_farguments_padding("padding", Padding);
		return 0;
	}();

	if (retCode != 0)
	{
		return PyErr_Format(PyExc_Exception, "could not add horizontal slot");
	}

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_shorizontal_box_num_slots(ue_PySHorizontalBox *self, PyObject * args)
{
	ue_py_slate_cast(SHorizontalBox);
	return PyLong_FromLong(py_SHorizontalBox->NumSlots());
}

static PyMethodDef ue_PySHorizontalBox_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_shorizontal_box_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ "num_slots", (PyCFunction)py_ue_shorizontal_box_num_slots, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(SHorizontalBox);

static int ue_py_shorizontal_box_init(ue_PySHorizontalBox *self, PyObject *args, PyObject *kwargs)
{
	ue_py_snew_simple(SHorizontalBox);
	return 0;
}

PyNumberMethods ue_PySHorizontalBox_number_methods;

static PyObject *ue_py_shorizontal_box_add(ue_PySHorizontalBox *self, PyObject *value)
{
	ue_py_slate_cast(SHorizontalBox);

	TSharedPtr<SWidget> Child = py_ue_is_swidget<SWidget>(value);
	if (!Child.IsValid())
	{
		return nullptr;
	}

	SHorizontalBox::FSlot &fslot = py_SHorizontalBox->AddSlot();
	fslot.AttachWidget(Child.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}

void ue_python_init_shorizontal_box(PyObject *ue_module)
{

	ue_PySHorizontalBoxType.tp_init = (initproc)ue_py_shorizontal_box_init;
	ue_PySHorizontalBoxType.tp_call = (ternaryfunc)py_ue_shorizontal_box_add_slot;

	memset(&ue_PySHorizontalBox_number_methods, 0, sizeof(PyNumberMethods));
	ue_PySHorizontalBoxType.tp_as_number = &ue_PySHorizontalBox_number_methods;
	ue_PySHorizontalBox_number_methods.nb_add = (binaryfunc)ue_py_shorizontal_box_add;

	ue_PySHorizontalBoxType.tp_base = &ue_PySBoxPanelType;

	if (PyType_Ready(&ue_PySHorizontalBoxType) < 0)
		return;

	Py_INCREF(&ue_PySHorizontalBoxType);
	PyModule_AddObject(ue_module, "SHorizontalBox", (PyObject *)&ue_PySHorizontalBoxType);
}
