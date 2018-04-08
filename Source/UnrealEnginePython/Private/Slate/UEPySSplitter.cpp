#include "UEPySSplitter.h"

static PyObject *py_ue_ssplitter_add_slot(ue_PySSplitter *self, PyObject * args, PyObject *kwargs)
{
	ue_py_slate_cast(SSplitter);
	PyObject *py_content;
	int index = -1;
	float size_value = -1;
	int sizing_rule = -1;

	char *kwlist[] = { (char *)"widget",
		(char *)"index",
		(char *)"size_value",
		(char *)"sizing_rule",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|ifi:add_slot", kwlist, &py_content, &index, &size_value, &sizing_rule))
	{
		return nullptr;
	}

	TSharedPtr<SWidget> Child = py_ue_is_swidget<SWidget>(py_content);
	if (!Child.IsValid())
	{
		return nullptr;
	}

	SSplitter::FSlot &fslot = py_SSplitter->AddSlot(index);
	if (size_value > -1)
	{
		fslot.SizeValue = size_value;
	}
	if (sizing_rule > -1)
	{
		fslot.SizingRule = (SSplitter::ESizeRule)sizing_rule;
	}
	fslot.AttachWidget(Child.ToSharedRef());

	Py_RETURN_SLATE_SELF;
}

static PyMethodDef ue_PySSplitter_methods[] = {
#pragma warning(suppress: 4191)
	{ "add_slot", (PyCFunction)py_ue_ssplitter_add_slot, METH_VARARGS | METH_KEYWORDS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySSplitterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SSplitter", /* tp_name */
	sizeof(ue_PySSplitter), /* tp_basicsize */
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
	"Unreal Engine SSplitter",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySSplitter_methods,             /* tp_methods */
};

static int ue_py_ssplitter_init(ue_PySSplitter *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SSplitter);

	ue_py_slate_farguments_optional_float("hit_detection_splitter_handle_size", HitDetectionSplitterHandleSize);
	ue_py_slate_farguments_event("on_splitter_finished_resizing", OnSplitterFinishedResizing, FSimpleDelegate, SimpleExecuteAction);
	ue_py_slate_farguments_optional_enum("orientation", Orientation, EOrientation);
	ue_py_slate_farguments_optional_float("physical_splitter_handle_size", PhysicalSplitterHandleSize);
	ue_py_slate_farguments_optional_enum("resize_mode", ResizeMode, ESplitterResizeMode::Type);
	ue_py_slate_farguments_optional_struct_ptr("style", Style, FSplitterStyle);

	ue_py_snew(SSplitter);
	return 0;
}

void ue_python_init_ssplitter(PyObject *ue_module)
{

	ue_PySSplitterType.tp_init = (initproc)ue_py_ssplitter_init;
	ue_PySSplitterType.tp_call = (ternaryfunc)py_ue_ssplitter_add_slot;

	ue_PySSplitterType.tp_base = &ue_PySPanelType;

	if (PyType_Ready(&ue_PySSplitterType) < 0)
		return;

	Py_INCREF(&ue_PySSplitterType);
	PyModule_AddObject(ue_module, "SSplitter", (PyObject *)&ue_PySSplitterType);
}
