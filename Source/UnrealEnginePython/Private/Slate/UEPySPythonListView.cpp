
#include "UEPySPythonListView.h"

#include "UEPySHeaderRow.h"


void SPythonListView::SetHeaderRow(TSharedPtr<SHeaderRow> InHeaderRowWidget)
{
	HeaderRow = InHeaderRowWidget;
}

static PyObject *py_ue_spython_list_view_get_selected_items(ue_PySPythonListView *self, PyObject * args)
{
	ue_py_slate_cast(SPythonListView);

	PyObject *py_list = PyList_New(0);

	TArray<TSharedPtr<FPythonItem>> items = py_SPythonListView->GetSelectedItems();

	for (auto item : items)
	{
		PyList_Append(py_list, item->py_object);
	}

	return py_list;
}

static PyObject *py_ue_spython_list_view_clear_selection(ue_PySPythonListView *self, PyObject * args)
{
	ue_py_slate_cast(SPythonListView);
	py_SPythonListView->ClearSelection();

	Py_RETURN_NONE;
}

static PyObject *py_ue_spython_list_view_get_num_items_selected(ue_PySPythonListView *self, PyObject * args)
{
	ue_py_slate_cast(SPythonListView);
	return PyLong_FromLong(py_SPythonListView->GetNumItemsSelected());
}

static PyObject *py_ue_spython_list_view_set_header_row(ue_PySPythonListView *self, PyObject * args)
{
	ue_py_slate_cast(SPythonListView);
	PyObject *py_content;
	if (!PyArg_ParseTuple(args, "O:set_header_row", &py_content))
	{
		return nullptr;
	}

	TSharedPtr<SHeaderRow> HeaderRow = py_ue_is_swidget<SHeaderRow>(py_content);
	if (!HeaderRow.IsValid())
	{
		return nullptr;
	}

	py_SPythonListView->SetHeaderRow(HeaderRow);

	Py_RETURN_SLATE_SELF;
}

static PyObject *py_spython_list_view_update_item_source_list(ue_PySPythonListView *self, PyObject * args)
{
	PyObject *values;
	if (!PyArg_ParseTuple(args, "O:update_item_source_list", &values))
	{
		return NULL;
	}

	values = PyObject_GetIter(values);
	if (!values)
	{
		return PyErr_Format(PyExc_Exception, "argument is not an iterable");
	}

	//NOTE: ikrimae: Increment first so we don't decrement and destroy python objects that 
	//we're passing in e.g. if you pass the same item source array into update_items(). 
	//Might not be necessary but I'm not too familiar with python's GC
	TArray<TSharedPtr<FPythonItem>> tempNewArray;
	while (PyObject *item = PyIter_Next(values))
	{
		Py_INCREF(item);
		tempNewArray.Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}

	for (TSharedPtr<struct FPythonItem>& item : self->item_source_list)
	{
		Py_XDECREF(item->py_object);
	}
	self->item_source_list.Empty();

	Move<TArray<TSharedPtr<FPythonItem>>>(self->item_source_list, tempNewArray);
	Py_RETURN_NONE;
}

static PyMethodDef ue_PySPythonListView_methods[] = {
	{ "get_selected_items", (PyCFunction)py_ue_spython_list_view_get_selected_items, METH_VARARGS, "" },
	{ "get_num_items_selected", (PyCFunction)py_ue_spython_list_view_get_num_items_selected, METH_VARARGS, "" },
	{ "clear_selection", (PyCFunction)py_ue_spython_list_view_clear_selection, METH_VARARGS, "" },
	{ "set_header_row", (PyCFunction)py_ue_spython_list_view_set_header_row, METH_VARARGS, "" },
	{ "update_item_source_list", (PyCFunction)py_spython_list_view_update_item_source_list, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static void ue_PySPythonListView_dealloc(ue_PySPythonListView *self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PySPythonListView %p"), self);
#endif

	for (TSharedPtr<struct FPythonItem>& item : self->item_source_list)
	{
		Py_XDECREF(item->py_object);
	}
	self->item_source_list.Empty();

	Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject ue_PySPythonListViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonListView", /* tp_name */
	sizeof(ue_PySPythonListView), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PySPythonListView_dealloc,       /* tp_dealloc */
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
	"Unreal Engine SPythonListView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonListView_methods,             /* tp_methods */
};

static int ue_py_spython_list_view_init(ue_PySPythonListView *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SPythonListView);

	// first of all check for values
	PyObject *values = ue_py_dict_get_item(kwargs, "list_items_source");
	if (!values)
	{
		PyErr_SetString(PyExc_Exception, "you must specify list items");
		return -1;
	}

	values = PyObject_GetIter(values);
	if (!values)
	{
		Py_DECREF(values);
		return -1;
	}

	new(&self->item_source_list) TArray<TSharedPtr<FPythonItem>>();
	while (PyObject *item = PyIter_Next(values))
	{
		Py_INCREF(item);
		self->item_source_list.Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}
	arguments.ListItemsSource(&self->item_source_list);

	{
		PyObject *value = ue_py_dict_get_item(kwargs, "header_row");
		if (value)
		{
			if (ue_PySHeaderRow *_py_swidget = py_ue_is_sheader_row(value))
			{
				arguments.HeaderRow(StaticCastSharedRef<SHeaderRow>(((ue_PySWidget *)_py_swidget)->Widget));
			}
			else
			{
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " "header_row");
				return -1;
			}
		}
	}

	ue_py_slate_farguments_optional_enum("allow_overscroll", AllowOverscroll, EAllowOverscroll);
	ue_py_slate_farguments_optional_bool("clear_selection_on_click", ClearSelectionOnClick);
	ue_py_slate_farguments_optional_enum("consume_mouse_wheel", ConsumeMouseWheel, EConsumeMouseWheel);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_bool("handle_gamepad_events", HandleGamepadEvents);
#endif
	ue_py_slate_farguments_float("item_height", ItemHeight);
	ue_py_slate_farguments_event("on_generate_row", OnGenerateRow, TSlateDelegates<TSharedPtr<FPythonItem>>::FOnGenerateRow, GenerateRow);
	ue_py_slate_farguments_event("on_selection_changed", OnSelectionChanged, TSlateDelegates<TSharedPtr<FPythonItem>>::FOnSelectionChanged, OnSelectionChanged);
	ue_py_slate_farguments_enum("selection_mode", SelectionMode, ESelectionMode::Type);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_float("wheel_scroll_multiplier", WheelScrollMultiplier);
#endif

	ue_py_snew(SPythonListView);
	return 0;
}

void ue_python_init_spython_list_view(PyObject *ue_module)
{

	ue_PySPythonListViewType.tp_init = (initproc)ue_py_spython_list_view_init;

	ue_PySPythonListViewType.tp_base = &ue_PySListViewType;

	if (PyType_Ready(&ue_PySPythonListViewType) < 0)
		return;

	Py_INCREF(&ue_PySPythonListViewType);
	PyModule_AddObject(ue_module, "SPythonListView", (PyObject *)&ue_PySPythonListViewType);
}