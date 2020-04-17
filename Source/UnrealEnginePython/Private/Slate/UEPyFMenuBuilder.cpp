#include "UEPyFMenuBuilder.h"

#include "Wrappers/UEPyESlateEnums.h"

static PyObject* py_ue_fmenu_builder_begin_section(ue_PyFMenuBuilder* self, PyObject* args)
{
	char* name;
	char* text;
	if (!PyArg_ParseTuple(args, "ss:begin_section", &name, &text))
		return nullptr;

	self->menu_builder.BeginSection(name, FText::FromString(UTF8_TO_TCHAR(text)));

	Py_RETURN_NONE;
}

static PyObject* py_ue_fmenu_builder_end_section(ue_PyFMenuBuilder* self, PyObject* args)
{
	self->menu_builder.EndSection();

	Py_RETURN_NONE;
}

static PyObject* py_ue_fmenu_builder_make_widget(ue_PyFMenuBuilder* self, PyObject* args)
{
	ue_PySWidget* ret = (ue_PySWidget*)PyObject_New(ue_PySWidget, &ue_PySWidgetType);
	new (&ret->Widget) TSharedRef<SWidget>(self->menu_builder.MakeWidget());
	return (PyObject*)ret;
}

static PyObject* py_ue_fmenu_builder_add_menu_entry(ue_PyFMenuBuilder* self, PyObject* args)
{
	char* label;
	char* tooltip;
	PyObject* py_callable;
	PyObject* py_obj = nullptr;
#if ENGINE_MINOR_VERSION >= 23
	int ui_action_type = (int)EUserInterfaceActionType::Button;
#else
	int ui_action_type = EUserInterfaceActionType::Button;
#endif
	if (!PyArg_ParseTuple(args, "ssO|Oi:add_menu_entry", &label, &tooltip, &py_callable, &py_obj, &ui_action_type))
		return nullptr;

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}


	FExecuteAction handler;
	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);

	if (py_obj)
	{
		Py_INCREF(py_obj);
		handler.BindSP(py_delegate, &FPythonSlateDelegate::ExecuteAction, py_obj);
	}
	else
	{
		handler.BindSP(py_delegate, &FPythonSlateDelegate::SimpleExecuteAction);
	}

	self->menu_builder.AddMenuEntry(FText::FromString(UTF8_TO_TCHAR(label)), FText::FromString(UTF8_TO_TCHAR(tooltip)), FSlateIcon(), FUIAction(handler), NAME_None,
#if ENGINE_MINOR_VERSION >= 23
		(EUserInterfaceActionType)ui_action_type);
#else
		(EUserInterfaceActionType::Type)ui_action_type);
#endif

	Py_RETURN_NONE;
}

static PyObject* py_ue_fmenu_builder_add_sub_menu(ue_PyFMenuBuilder* self, PyObject* args)
{
	char* label;
	char* tooltip;
	PyObject* py_callable;
	PyObject* py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "ssO|O:add_sub_menu", &label, &tooltip, &py_callable, &py_bool))
		return nullptr;

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}


	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);

	FNewMenuDelegate menu_delegate;
	menu_delegate.BindSP(py_delegate, &FPythonSlateDelegate::SubMenuPyBuilder);


	self->menu_builder.AddSubMenu(FText::FromString(UTF8_TO_TCHAR(label)), FText::FromString(UTF8_TO_TCHAR(tooltip)), menu_delegate, (py_bool && PyObject_IsTrue(py_bool)) ? true : false);

	Py_RETURN_NONE;
}

static PyObject* py_ue_fmenu_builder_add_menu_separator(ue_PyFMenuBuilder* self, PyObject* args)
{
	char* name = nullptr;

	if (!PyArg_ParseTuple(args, "|s:add_menu_separator", &name))
		return nullptr;

	FName f_name = NAME_None;

	if (name)
		f_name = FName(UTF8_TO_TCHAR(name));

	self->menu_builder.AddMenuSeparator(f_name);

	Py_RETURN_NONE;
}

#if WITH_EDITOR
static PyObject* py_ue_fmenu_builder_add_asset_actions(ue_PyFMenuBuilder* self, PyObject* args)
{
	PyObject* py_assets;

	if (!PyArg_ParseTuple(args, "O:add_asset_actions", &py_assets))
		return nullptr;

	py_assets = PyObject_GetIter(py_assets);
	if (!py_assets)
	{
		return PyErr_Format(PyExc_Exception, "argument is not iterable");
	}

	TArray<UObject*> u_objects;
	while (PyObject * item = PyIter_Next(py_assets))
	{
		UObject* u_object = ue_py_check_type<UObject>(item);
		if (u_object)
		{
			u_objects.Add(u_object);
		}
	}
	Py_DECREF(py_assets);

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	bool addedSomething = AssetToolsModule.Get().GetAssetActions(u_objects, self->menu_builder, true);
	if (addedSomething)
	{
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}
#endif

static PyObject* py_ue_fmenu_builder_add_search_widget(ue_PyFMenuBuilder* self, PyObject* args)
{
	self->menu_builder.AddSearchWidget();

	Py_RETURN_NONE;
}

static PyMethodDef ue_PyFMenuBuilder_methods[] = {
	{ "begin_section", (PyCFunction)py_ue_fmenu_builder_begin_section, METH_VARARGS, "" },
	{ "end_section", (PyCFunction)py_ue_fmenu_builder_end_section, METH_VARARGS, "" },
	{ "make_widget", (PyCFunction)py_ue_fmenu_builder_make_widget, METH_VARARGS, "" },
	{ "add_menu_entry", (PyCFunction)py_ue_fmenu_builder_add_menu_entry, METH_VARARGS, "" },
	{ "add_menu_separator", (PyCFunction)py_ue_fmenu_builder_add_menu_separator, METH_VARARGS, "" },
	{ "add_search_widget", (PyCFunction)py_ue_fmenu_builder_add_search_widget, METH_VARARGS, "" },
	{ "add_sub_menu", (PyCFunction)py_ue_fmenu_builder_add_sub_menu, METH_VARARGS, "" },
#if WITH_EDITOR
	{ "add_asset_actions", (PyCFunction)py_ue_fmenu_builder_add_asset_actions, METH_VARARGS, "" },
#endif
	{ NULL }  /* Sentinel */
};


static PyObject* ue_PyFMenuBuilder_str(ue_PyFMenuBuilder* self)
{
	return PyUnicode_FromFormat("<unreal_engine.FMenuBuilder '%p'}>",
		&self->menu_builder);
}

static void ue_py_fmenu_builder_dealloc(ue_PyFMenuBuilder* self)
{
#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyTypeObject ue_PyFMenuBuilderType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FMenuBuilder", /* tp_name */
	sizeof(ue_PyFMenuBuilder), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_fmenu_builder_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyFMenuBuilder_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FMenuBuilder",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFMenuBuilder_methods,             /* tp_methods */
};

static int ue_py_fmenu_builder_init(ue_PyFMenuBuilder* self, PyObject* args, PyObject* kwargs)
{
	new(&self->menu_builder) FMenuBuilder(true, nullptr);
	return 0;
}


void ue_python_init_fmenu_builder(PyObject* ue_module)
{
	ue_PyFMenuBuilderType.tp_new = PyType_GenericNew;

	ue_PyFMenuBuilderType.tp_init = (initproc)ue_py_fmenu_builder_init;

	if (PyType_Ready(&ue_PyFMenuBuilderType) < 0)
		return;

	Py_INCREF(&ue_PyFMenuBuilderType);
	PyModule_AddObject(ue_module, "FMenuBuilder", (PyObject*)& ue_PyFMenuBuilderType);
}

PyObject* py_ue_new_fmenu_builder(FMenuBuilder menu_builder)
{
	ue_PyFMenuBuilder* ret = (ue_PyFMenuBuilder*)PyObject_New(ue_PyFMenuBuilder, &ue_PyFMenuBuilderType);
	new(&ret->menu_builder) FMenuBuilder(menu_builder);
	return (PyObject*)ret;
}
