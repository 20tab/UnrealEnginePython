
#include "UEPySDockTab.h"

#include "UEPyFTabManager.h"

static PyObject *py_ue_sdock_tab_set_label(ue_PySDockTab *self, PyObject * args)
{
	ue_py_slate_cast(SDockTab);
	char *label;
	if (!PyArg_ParseTuple(args, "s:set_label", &label))
	{
		return nullptr;
	}

	py_SDockTab->SetLabel(FText::FromString(UTF8_TO_TCHAR(label)));

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_ue_sdock_tab_request_close_tab(ue_PySDockTab *self, PyObject * args)
{
	ue_py_slate_cast(SDockTab);
	py_SDockTab->RequestCloseTab();

	Py_RETURN_NONE;
}

static PyObject *py_ue_sdock_tab_new_tab_manager(ue_PySDockTab *self, PyObject * args)
{
	ue_py_slate_cast(SDockTab);
	TSharedRef<FTabManager> tab_manager = FGlobalTabmanager::Get()->NewTabManager(py_SDockTab);

	return py_ue_new_ftab_manager(tab_manager);
}

static PyMethodDef ue_PySDockTab_methods[] = {
	{ "set_label", (PyCFunction)py_ue_sdock_tab_set_label, METH_VARARGS, "" },
	{ "request_close_tab", (PyCFunction)py_ue_sdock_tab_request_close_tab, METH_VARARGS, "" },
	{ "new_tab_manager", (PyCFunction)py_ue_sdock_tab_new_tab_manager, METH_VARARGS, "" },
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

static int ue_py_sdock_tab_init(ue_PySDockTab *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SDockTab);

	ue_py_slate_farguments_struct("content_padding", ContentPadding, FMargin);
	ue_py_slate_farguments_optional_struct_ptr("icon", Icon, FSlateBrush);
	ue_py_slate_farguments_text("label", Label);
	ue_py_slate_farguments_optional_bool("should_autosize", ShouldAutosize);
	ue_py_slate_farguments_optional_enum("tab_role", TabRole, ETabRole);


	ue_py_snew(SDockTab);
	return 0;
}

void ue_python_init_sdock_tab(PyObject *ue_module)
{

	ue_PySDockTabType.tp_base = &ue_PySBorderType;
	ue_PySDockTabType.tp_init = (initproc)ue_py_sdock_tab_init;

	if (PyType_Ready(&ue_PySDockTabType) < 0)
		return;

	Py_INCREF(&ue_PySDockTabType);
	PyModule_AddObject(ue_module, "SDockTab", (PyObject *)&ue_PySDockTabType);
}
