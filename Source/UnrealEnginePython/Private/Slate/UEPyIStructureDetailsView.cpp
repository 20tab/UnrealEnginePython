
#include "UEPyIStructureDetailsView.h"

#if WITH_EDITOR


#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "UEPySWidget.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"


static PyObject *ue_PyIStructureDetailsView_str(ue_PyIStructureDetailsView *self)
{
#if PY_MAJOR_VERSION >= 3
	return PyUnicode_FromFormat("<unreal_engine.%s '%p' (slate ref count: %d, py ref count: %d)>",
		TCHAR_TO_UTF8(*self->istructure_details_view->GetWidget()->GetTypeAsString()),
		self->istructure_details_view->GetWidget().Get(),
		self->istructure_details_view->GetWidget().GetSharedReferenceCount(),
		self->ob_base.ob_refcnt);
#else
	return PyUnicode_FromFormat("<unreal_engine.%s '%p' (slate ref count: %d)>",
		TCHAR_TO_UTF8(*self->istructure_details_view->GetWidget()->GetTypeAsString()),
		self->istructure_details_view->GetWidget().Get(),
		self->istructure_details_view->GetWidget().GetSharedReferenceCount());
#endif
}


static PyObject *py_ue_istructure_details_view_set_structure_data(ue_PyIStructureDetailsView *self, PyObject * args, PyObject *kwargs)
{
	PyObject *py_object = nullptr;
	PyObject *py_force_refresh = nullptr;

	char *kwlist[] = {
		(char *)"struct_data",
		(char *)"force_refresh",
		nullptr
	};

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O|O:set_structure_data", kwlist, &py_object, &py_force_refresh))
	{
		return nullptr;
	}

	ue_PyUScriptStruct *ue_py_struct = py_ue_is_uscriptstruct(py_object);
	if (!ue_py_struct)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UScriptStruct");
	}


	Py_XDECREF(self->ue_py_struct);
	self->ue_py_struct = ue_py_struct;
	Py_INCREF(self->ue_py_struct);
	TSharedPtr<FStructOnScope> struct_scope = MakeShared<FStructOnScope>(ue_py_struct->u_struct, ue_py_struct->u_struct_ptr);
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	self->istructure_details_view->SetStructureData(struct_scope);

#if ENGINE_MINOR_VERSION > 17
	if (py_force_refresh && PyObject_IsTrue(py_force_refresh))
	{
		self->istructure_details_view->GetDetailsView()->ForceRefresh();
}
#endif

	Py_RETURN_NONE;
}

static PyObject *py_ue_istructure_details_view_get_widget(ue_PyIStructureDetailsView *self, PyObject * args)
{

	TSharedPtr<SWidget> ret_widget = self->istructure_details_view->GetWidget();
	if (!ret_widget.IsValid())
	{
		return PyErr_Format(PyExc_Exception, "unable to create SingleProperty widget");
	}

	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(ret_widget->AsShared(), &ue_PySWidgetType);

	Py_RETURN_NONE;
}

static PyMethodDef ue_PyIStructureDetailsView_methods[] = {
#pragma warning(suppress: 4191)
	{ "set_structure_data", (PyCFunction)py_ue_istructure_details_view_set_structure_data, METH_VARARGS | METH_KEYWORDS, "" },
	{ "get_widget", (PyCFunction)py_ue_istructure_details_view_get_widget, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static void ue_PyIStructureDetailsView_dealloc(ue_PyIStructureDetailsView *self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyIStructureDetailsView %p mapped to IStructureDetailsView %p"), self, self->istructure_details_view.Get());
#endif
	Py_DECREF(self->ue_py_struct);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject ue_PyIStructureDetailsViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IStructureDetailsView", /* tp_name */
	sizeof(ue_PyIStructureDetailsView), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PyIStructureDetailsView_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyIStructureDetailsView_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine IStructureDetailsView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIStructureDetailsView_methods,             /* tp_methods */
};

static int ue_py_istructure_details_view_init(ue_PyIStructureDetailsView *self, PyObject *args, PyObject *kwargs)
{

	PyObject *py_object = nullptr;

	PyObject *py_allow_search = nullptr;
	PyObject *py_update_from_selection = nullptr;
	PyObject *py_lockable = nullptr;
	char     *py_name_area_settings = nullptr;
	PyObject *py_hide_selection_tip = nullptr;
	PyObject *py_search_initial_key_focus = nullptr;

	char *kwlist[] = {
		(char*)"struct_data",
		(char *)"allow_search",
		(char *)"update_from_selection",
		(char *)"lockable",
		(char *)"name_area_settings",
		(char *)"hide_selection_tip",
		(char *)"search_initial_key_focus",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOsOO:__init__", kwlist,
		&py_object, &py_allow_search, &py_update_from_selection, &py_lockable, &py_name_area_settings, &py_hide_selection_tip, &py_search_initial_key_focus))
	{
		return -1;
	}

	ue_PyUScriptStruct *ue_py_struct = py_ue_is_uscriptstruct(py_object);
	if (!ue_py_struct)
	{
		PyErr_SetString(PyExc_Exception, "argument is not a UScriptStruct");
		return -1;
	}

	FDetailsViewArgs view_args;
	view_args.bAllowSearch = (py_allow_search) ? PyObject_IsTrue(py_allow_search) : view_args.bAllowSearch;
	view_args.bUpdatesFromSelection = (py_update_from_selection) ? PyObject_IsTrue(py_update_from_selection) : view_args.bUpdatesFromSelection;
	view_args.bLockable = (py_lockable) ? PyObject_IsTrue(py_lockable) : view_args.bLockable;
	view_args.bHideSelectionTip = (py_hide_selection_tip) ? PyObject_IsTrue(py_hide_selection_tip) : view_args.bHideSelectionTip;
	view_args.bSearchInitialKeyFocus = (py_search_initial_key_focus) ? PyObject_IsTrue(py_search_initial_key_focus) : view_args.bSearchInitialKeyFocus;

	FString name_area_string = py_name_area_settings ? FString(UTF8_TO_TCHAR(py_name_area_settings)) : FString();
	view_args.NameAreaSettings = [&name_area_string]() {
		if (FCString::Stricmp(*name_area_string, TEXT("HideNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::HideNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ObjectsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ObjectsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ComponentsAndActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ComponentsAndActorsUseNameArea; }
		else { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
	}();
	FStructureDetailsViewArgs struct_view_args;
	{
		struct_view_args.bShowObjects = true;
		struct_view_args.bShowAssets = true;
		struct_view_args.bShowClasses = true;
		struct_view_args.bShowInterfaces = true;
	}
	new(&self->istructure_details_view) TSharedPtr<IStructureDetailsView>(nullptr);

	self->ue_py_struct = ue_py_struct;
	Py_INCREF(self->ue_py_struct);
	TSharedPtr<FStructOnScope> struct_scope = MakeShared<FStructOnScope>(ue_py_struct->u_struct, ue_py_struct->u_struct_ptr);
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	new(&self->istructure_details_view) TSharedRef<IStructureDetailsView>(PropertyEditorModule.CreateStructureDetailView(view_args, struct_view_args, struct_scope));

	return 0;
}

void ue_python_init_istructure_details_view(PyObject *ue_module)
{
	ue_PyIStructureDetailsViewType.tp_new = PyType_GenericNew;

	ue_PyIStructureDetailsViewType.tp_init = (initproc)ue_py_istructure_details_view_init;

	ue_PyIStructureDetailsViewType.tp_getattro = PyObject_GenericGetAttr;
	ue_PyIStructureDetailsViewType.tp_setattro = PyObject_GenericSetAttr;

	if (PyType_Ready(&ue_PyIStructureDetailsViewType) < 0)
		return;

	Py_INCREF(&ue_PyIStructureDetailsViewType);
	PyModule_AddObject(ue_module, "IStructureDetailsView", (PyObject *)&ue_PyIStructureDetailsViewType);
}

ue_PyIStructureDetailsView *py_ue_is_istructure_details_view(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyIStructureDetailsViewType))
		return nullptr;
	return (ue_PyIStructureDetailsView *)obj;
}
#endif