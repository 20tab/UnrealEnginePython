
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySDockTab.h"
#include "SDockTab.h"
#include "Private/Framework/Docking/SDockingArea.h"
#include "SlateWindowHelper.h"
#include "WidgetPath.h"
#include "IMenu.h"
#include "SlateApplication.h"

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

static PyObject *py_ue_sdock_tab_set_on_tab_closed(ue_PySButton *self, PyObject * args) 
{
    ue_py_slate_cast(SDockTab);

	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:set_on_tab_closed", &py_callable))
	{
		return NULL;
	}

	if (!PyCalllable_Check_Extended(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	SDockTab::FOnTabClosedCallback onTabClosed;
	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);
	onTabClosed.BindSP(py_delegate, &FPythonSlateDelegate::OnTabClosed);

	py_SDockTab->SetOnTabClosed(onTabClosed);
	Py_RETURN_NONE;
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


static PyObject *py_ue_sdock_tab_bring_to_front(ue_PySDockTab *self, PyObject * args)
{
    ue_py_slate_cast(SDockTab);
	TSharedPtr<SWindow> parentWindow = py_SDockTab->GetParentWindow();
	if (parentWindow.Get() != nullptr)
	{
		parentWindow->HACK_ForceToFront();
	}
	Py_RETURN_NONE;
}

static PyObject *py_ue_sdock_get_parent_window(ue_PySDockTab *self, PyObject * args)
{
    ue_py_slate_cast(SDockTab);
	TSharedPtr<SWindow> ParentWindow = py_SDockTab->GetParentWindow();
	if (ParentWindow.IsValid())
	{
		// Check to see if the widget exists already
		ue_PySWidget *ret = ue_py_get_swidget(StaticCastSharedRef<SWidget>(ParentWindow.ToSharedRef()));
		return (PyObject *)ret;
	}
    Py_RETURN_NONE;
}

static PyObject *py_ue_sdock_get_docking_area(ue_PySDockTab *self, PyObject * args)
{
    ue_py_slate_cast(SDockTab);
    TSharedPtr<SDockingArea> DockingArea = py_SDockTab->GetDockArea();
	if (DockingArea.IsValid())
	{
		ue_PySWidget *ret = nullptr;
		// Checks to see if the widget exists already, is added to mapping if not
		ret = ue_py_get_swidget(StaticCastSharedRef<SWidget>(DockingArea.ToSharedRef()));
		return (PyObject *)ret;
	}
    Py_RETURN_NONE;
}

static PyMethodDef ue_PySDockTab_methods[] = {
	{ "set_label", (PyCFunction)py_ue_sdock_tab_set_label, METH_VARARGS, "" },
	{ "set_on_tab_closed", (PyCFunction)py_ue_sdock_tab_set_on_tab_closed, METH_VARARGS, "" },
	{ "request_close_tab", (PyCFunction)py_ue_sdock_tab_request_close_tab, METH_VARARGS, "" },
	{ "bring_to_front", (PyCFunction)py_ue_sdock_tab_bring_to_front, METH_VARARGS, "" },
	{ "new_tab_manager", (PyCFunction)py_ue_sdock_tab_new_tab_manager, METH_VARARGS, "" },
	{ "get_parent_window", (PyCFunction)py_ue_sdock_get_parent_window, METH_VARARGS, "" },
	{ "get_docking_area", (PyCFunction)py_ue_sdock_get_docking_area, METH_VARARGS, "" },
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

    ue_py_slate_farguments_default_slot           ("content",                     Content);                   
    ue_py_slate_farguments_named_slot             ("tab_well_content_left",       TabWellContentLeft);
    ue_py_slate_farguments_named_slot             ("tab_well_content_right",      TabWellContentRight);
    ue_py_slate_farguments_named_slot             ("tab_well_content_background", TabWellContentBackground);
    ue_py_slate_farguments_attribute_struct       ("content_padding",             ContentPadding, FMargin);
    ue_py_slate_farguments_argument_enum          ("tab_role",                    TabRole, ETabRole);
    ue_py_slate_farguments_attribute_text         ("label",                       Label);
    ue_py_slate_farguments_argument_struct_ptr    ("icon",                        Icon, FSlateBrush);
    ue_py_slate_farguments_attribute_event        ("on_tab_closed",               OnTabClosed, SDockTab::FOnTabClosedCallback, OnTabClosed);
    ue_py_slate_farguments_attribute_event        ("on_tab_activated",            OnTabActivated, SDockTab::FOnTabActivatedCallback, OnTabActivated);
    ue_py_slate_farguments_argument_bool          ("should_autosize",             ShouldAutosize);
    ue_py_slate_farguments_attribute_event        ("on_can_close_tab",            OnCanCloseTab, SDockTab::FCanCloseTab, OnCanCloseTab);
    ue_py_slate_farguments_attribute_event        ("on_persist_visual_state",     OnPersistVisualState, SDockTab::FOnPersistVisualState, OnPersistVisualState);
    ue_py_slate_farguments_attribute_flinear_color("tab_color_scale",             TabColorScale);

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

ue_PySDockTab* py_ue_is_sdock_tab(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PySDockTabType))
		return nullptr;
	return (ue_PySDockTab* )obj;
}
