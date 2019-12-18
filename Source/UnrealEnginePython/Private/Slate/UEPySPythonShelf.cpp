#include "UEPySPythonShelf.h"
#if WITH_EDITOR

#if ENGINE_MINOR_VERSION > 14



#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"

#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"
#include "Editor/ContentBrowser/Private/SAssetPicker.h"


static PyMethodDef ue_PySPythonShelf_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySPythonShelfType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonShelf", /* tp_name */
	sizeof(ue_PySPythonShelf), /* tp_basicsize */
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
	"Unreal Engine SPythonShelf",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonShelf_methods,             /* tp_methods */
};

static int ue_py_spython_shelf_init(ue_PySPythonShelf *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_classes_iterable = nullptr;
	PyObject *py_collections_iterable = nullptr;

	PyObject *py_callable_double_clicked = nullptr;
	PyObject *py_callable_get_context_menu = nullptr;
	PyObject *py_callable_asset_selected = nullptr;

	char *kwlist[] = { (char *)"classes",
		(char *)"collections",
		(char *)"on_asset_double_clicked",
		(char *)"on_get_asset_context_menu",
		(char *)"on_asset_selected",
		nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOO", kwlist,
		&py_classes_iterable,
		&py_collections_iterable,
		&py_callable_double_clicked,
		&py_callable_get_context_menu,
		&py_callable_asset_selected))
	{
		return -1;
	}

	if (py_classes_iterable)
	{
		py_classes_iterable = PyObject_GetIter(py_classes_iterable);
		if (!py_classes_iterable)
		{
			PyErr_SetString(PyExc_Exception, "argument is not an iterable");
			return -1;
		}
	}

	if (py_collections_iterable)
	{
		py_collections_iterable = PyObject_GetIter(py_collections_iterable);
		if (!py_collections_iterable)
		{
			PyErr_SetString(PyExc_Exception, "argument is not an iterable");
			return -1;
		}
	}

	if (py_callable_double_clicked && !PyCallable_Check(py_callable_double_clicked))
	{
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return -1;
	}

	if (py_callable_get_context_menu && !PyCallable_Check(py_callable_get_context_menu))
	{
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return -1;
	}

	if (py_callable_asset_selected && !PyCallable_Check(py_callable_asset_selected))
	{
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return -1;
	}

	FContentBrowserModule& Module = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	FAssetPickerConfig asset_picker_config;
	asset_picker_config.InitialAssetViewType = EAssetViewType::Tile;
	asset_picker_config.bAllowNullSelection = false;
	asset_picker_config.bShowBottomToolbar = false;
	asset_picker_config.bAutohideSearchBar = false;

	if (py_classes_iterable)
	{
		while (PyObject *item = PyIter_Next(py_classes_iterable))
		{
			if (PyUnicodeOrString_Check(item))
			{
				FName class_name = FName(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(item)));
				asset_picker_config.Filter.ClassNames.Add(class_name);
			}
		}
		Py_DECREF(py_classes_iterable);
	}

	if (py_collections_iterable)
	{
		while (PyObject *item = PyIter_Next(py_collections_iterable))
		{
			if (PyUnicodeOrString_Check(item))
			{
				FName collection_name = FName(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(item)));
				asset_picker_config.Collections.Add(FCollectionNameType(collection_name, ECollectionShareType::CST_Local));
			}
		}
		Py_DECREF(py_collections_iterable);
	}

	if (py_callable_double_clicked)
	{
		FOnAssetDoubleClicked handler;
		TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable_double_clicked);
		handler.BindSP(py_delegate, &FPythonSlateDelegate::OnAssetDoubleClicked);


		asset_picker_config.OnAssetDoubleClicked = handler;
	}

	if (py_callable_get_context_menu)
	{
		FOnGetAssetContextMenu handler;
		TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable_get_context_menu);

		handler.BindSP(py_delegate, &FPythonSlateDelegate::OnGetAssetContextMenu);

		asset_picker_config.OnGetAssetContextMenu = handler;
	}

	if (py_callable_asset_selected)
	{
		FOnAssetSelected handler;
		TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable_asset_selected);

		handler.BindSP(py_delegate, &FPythonSlateDelegate::OnAssetSelected);

		asset_picker_config.OnAssetSelected = handler;
	}

	new(&self->s_compound_widget.s_widget.Widget) TSharedRef<SWidget>(Module.Get().CreateAssetPicker(asset_picker_config));

	return 0;
}

void ue_python_init_spython_shelf(PyObject *ue_module)
{

	ue_PySPythonShelfType.tp_init = (initproc)ue_py_spython_shelf_init;

	ue_PySPythonShelfType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySPythonShelfType) < 0)
		return;

	Py_INCREF(&ue_PySPythonShelfType);
	PyModule_AddObject(ue_module, "SPythonShelf", (PyObject *)&ue_PySPythonShelfType);
}
#endif
#endif