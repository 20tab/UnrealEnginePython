#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonShelf.h"

#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#include "Editor/ContentBrowser/Public/IContentBrowserSingleton.h"

static PyObject *ue_PySPythonShelf_str(ue_PySPythonShelf *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SPythonShelf '%p'>",
		self->s_compound_widget.s_widget.s_widget);
}

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
	(reprfunc)ue_PySPythonShelf_str,                         /* tp_str */
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

static int ue_py_spython_shelf_init(ue_PySPythonShelf *self, PyObject *args, PyObject *kwargs) {
	PyObject *py_classes_iterable = nullptr;
	PyObject *py_collections_iterable = nullptr;

	PyObject *py_callable_double_clicked = nullptr;
	PyObject *py_callable_get_context_menu = nullptr;

	char *kwlist[] = { (char *)"classes", (char *)"collections", (char *)"on_asset_double_clicked", (char *)"on_get_asset_context_menu", nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOO", kwlist,
		&py_classes_iterable,
		&py_collections_iterable,
		&py_callable_double_clicked,
		&py_callable_get_context_menu)) {
		return -1;
	}

	if (py_classes_iterable) {
		py_classes_iterable = PyObject_GetIter(py_classes_iterable);
		if (!py_classes_iterable) {
			PyErr_SetString(PyExc_Exception, "argument is not an iterable");
			return -1;
		}
	}

	if (py_collections_iterable) {
		py_collections_iterable = PyObject_GetIter(py_collections_iterable);
		if (!py_collections_iterable) {
			PyErr_SetString(PyExc_Exception, "argument is not an iterable");
			return -1;
		}
	}

	if (py_callable_double_clicked && !PyCallable_Check(py_callable_double_clicked)) {
		PyErr_SetString(PyExc_Exception, "argument is not callable");
		return -1;
	}

	FContentBrowserModule& module = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));

	FAssetPickerConfig asset_picker_config;
	asset_picker_config.InitialAssetViewType = EAssetViewType::Tile;
	asset_picker_config.bAllowNullSelection = false;
	asset_picker_config.bShowBottomToolbar = false;
	asset_picker_config.bAutohideSearchBar = false;

	if (py_classes_iterable) {
		while (PyObject *item = PyIter_Next(py_classes_iterable)) {
			if (PyUnicode_Check(item)) {
				FName class_name = FName(UTF8_TO_TCHAR(PyUnicode_AsUTF8(item)));
				asset_picker_config.Filter.ClassNames.Add(class_name);
			}
		}
		Py_DECREF(py_classes_iterable);
	}

	if (py_collections_iterable) {
		while (PyObject *item = PyIter_Next(py_collections_iterable)) {
			if (PyUnicode_Check(item)) {
				FName collection_name = FName(UTF8_TO_TCHAR(PyUnicode_AsUTF8(item)));
				asset_picker_config.Collections.Add(FCollectionNameType(collection_name, ECollectionShareType::CST_Local));
			}
		}
		Py_DECREF(py_collections_iterable);
	}

	if (py_callable_double_clicked) {
		FOnAssetDoubleClicked handler;
		UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
		py_delegate->SetPyCallable(py_callable_double_clicked);
		py_delegate->AddToRoot();
		handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnAssetDoubleClicked);

		asset_picker_config.OnAssetDoubleClicked = handler;
	}


	self->s_compound_widget.s_widget.s_widget_owned = module.Get().CreateAssetPicker(asset_picker_config);
	self->s_compound_widget.s_widget.s_widget = &self->s_compound_widget.s_widget.s_widget_owned.Get();
	return 0;
}

void ue_python_init_spython_shelf(PyObject *ue_module) {
	ue_PySPythonShelfType.tp_new = PyType_GenericNew;

	ue_PySPythonShelfType.tp_init = (initproc)ue_py_spython_shelf_init;

	ue_PySPythonShelfType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySPythonShelfType) < 0)
		return;

	Py_INCREF(&ue_PySPythonShelfType);
	PyModule_AddObject(ue_module, "SPythonShelf", (PyObject *)&ue_PySPythonShelfType);
}


#endif