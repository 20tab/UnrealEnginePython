
#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/Slate/Public/Widgets/Views/STreeView.h"

#include "UEPySPythonTreeView.h"


TSharedPtr<struct FPythonItem> const* SPythonTreeView::Find(PyObject *item)
{
    for (const TSharedPtr<FPythonItem>& PythonItem : *ItemsSource)
    {
        if (PythonItem->py_object == item)
        {
            return &PythonItem;
        }
    }
    return nullptr;
}


static PyObject *py_ue_spython_tree_view_set_item_expansion(ue_PySPythonTreeView *self, PyObject * args)
{
	ue_py_slate_cast(SPythonTreeView);
	PyObject *py_item;
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "OO:set_item_expansion", &py_item, &py_bool))
	{
		return nullptr;
	}

    if (TSharedPtr<struct FPythonItem> const* foundItem = py_SPythonTreeView->Find(py_item))
    {
        py_SPythonTreeView->SetItemExpansion(*foundItem, PyObject_IsTrue(py_bool) ? true : false);
    }

	Py_RETURN_NONE;
}

static PyObject *py_ue_spython_tree_view_is_item_expanded(ue_PySPythonTreeView *self, PyObject * args)
{
    ue_py_slate_cast(SPythonTreeView);

	PyObject *py_item;
	if (!PyArg_ParseTuple(args, "O:is_item_expanded", &py_item))
	{
		return nullptr;
	}

    if (TSharedPtr<struct FPythonItem> const* foundItem = py_SPythonTreeView->Find(py_item))
    {
		bool bItemExpanded = py_SPythonTreeView->IsItemExpanded(*foundItem);
		if (bItemExpanded)
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
    }

	Py_RETURN_NONE;
}

static PyObject *py_ue_spython_tree_view_get_selected_items(ue_PySPythonTreeView *self, PyObject * args) 
{
    ue_py_slate_cast(SPythonTreeView);

	PyObject *py_list = PyList_New(0);

	TArray<TSharedPtr<FPythonItem>> items = py_SPythonTreeView->GetSelectedItems();

	for (TSharedPtr<FPythonItem>& item : items) {
		PyList_Append(py_list, item->py_object);
	}

	return py_list;
}

static PyObject *py_ue_spython_tree_view_get_expanded_items(ue_PySPythonTreeView *self, PyObject * args) 
{
    ue_py_slate_cast(SPythonTreeView);

	PyObject *py_list = PyList_New(0);

	TSet <TSharedPtr<FPythonItem>> ExpandedItems;
    py_SPythonTreeView->GetExpandedItems(ExpandedItems);

	for (TSharedPtr<FPythonItem>& item : ExpandedItems) {
		PyList_Append(py_list, item->py_object);
	}

	return py_list;
}

static PyObject *py_ue_spython_tree_view_clear_selection(ue_PySPythonTreeView *self, PyObject * args)
{
    ue_py_slate_cast(SPythonTreeView);

    py_SPythonTreeView->ClearSelection();

    Py_RETURN_NONE;
}

static PyObject *py_ue_spython_tree_view_get_num_items_selected(ue_PySPythonTreeView *self, PyObject * args) 
{
    ue_py_slate_cast(SPythonTreeView);
	return PyLong_FromLong(py_SPythonTreeView->GetNumItemsSelected());
}

static PyObject *py_ue_spython_tree_view_update_item_source_list(ue_PySPythonTreeView *self, PyObject * args)
{
    ue_py_slate_cast(SPythonTreeView);

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

	TSet<TSharedPtr<FPythonItem>> expanded_items;
    py_SPythonTreeView->GetExpandedItems(expanded_items);

    //NOTE: ikrimae: Increment first so we don't decrement and destroy python objects that 
    //we're passing in e.g. if you pass the same item source array into update_items(). 
    //Might not be necessary but I'm not too familiar with python's GC
    TArray<TSharedPtr<FPythonItem>> tempNewArray;
    while (PyObject *item = PyIter_Next(values)) {
        Py_INCREF(item);
        tempNewArray.Add(MakeShared<FPythonItem>(item));
	}

    for (TSharedPtr<FPythonItem>& item : self->item_source_list)
{
        Py_XDECREF(item->py_object);
    }
    self->item_source_list.Empty();

    Move<TArray<TSharedPtr<FPythonItem>>>(self->item_source_list, tempNewArray);
    
	for (TSharedPtr<FPythonItem>& old_item : expanded_items)
	{
        for (TSharedPtr<FPythonItem>& new_item : self->item_source_list)
        {
            if (old_item->py_object == new_item->py_object)
            {
                py_SPythonTreeView->SetItemExpansion(new_item, true);
            }
        }
    }

	Py_RETURN_NONE;
}

static PyMethodDef ue_PySPythonTreeView_methods[] = {
    { "get_selected_items", (PyCFunction)     py_ue_spython_tree_view_get_selected_items, METH_VARARGS, "" },
	{ "get_expanded_items", (PyCFunction)py_ue_spython_tree_view_get_expanded_items, METH_VARARGS, "" },
    { "get_num_items_selected", (PyCFunction) py_ue_spython_tree_view_get_num_items_selected, METH_VARARGS, "" },
    { "clear_selection", (PyCFunction)        py_ue_spython_tree_view_clear_selection, METH_VARARGS, "" },
    { "update_item_source_list", (PyCFunction)py_ue_spython_tree_view_update_item_source_list, METH_VARARGS, "" },
	{ "set_item_expansion", (PyCFunction)py_ue_spython_tree_view_set_item_expansion, METH_VARARGS, "" },
    { "is_item_expanded", (PyCFunction)py_ue_spython_tree_view_is_item_expanded, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static void ue_PySPythonTreeView_dealloc(ue_PySPythonTreeView *self)
{
#if defined(UEPY_MEMORY_DEBUG)
    UE_LOG(LogPython, Warning, TEXT("Destroying ue_PySPythonTreeView %p"), self);
#endif

    for (TSharedPtr<struct FPythonItem>& item : self->item_source_list)
    {
        Py_XDECREF(item->py_object);
    }
    self->item_source_list.Empty();

    Py_TYPE(self)->tp_free((PyObject *)self);
}

PyTypeObject ue_PySPythonTreeViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonTreeView", /* tp_name */
	sizeof(ue_PySPythonTreeView), /* tp_basicsize */
	0,                         /* tp_itemsize */
    (destructor)ue_PySPythonTreeView_dealloc,       /* tp_dealloc */
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
	"Unreal Engine SPythonTreeView",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonTreeView_methods,             /* tp_methods */
};

static int ue_py_spython_tree_view_init(ue_PySPythonTreeView *self, PyObject *args, PyObject *kwargs)
{

	ue_py_slate_setup_farguments(SPythonTreeView);

	// first of all check for values
	PyObject *values = ue_py_dict_get_item(kwargs, "tree_items_source");
	if (!values)
	{
		PyErr_SetString(PyExc_Exception, "you must specify tree items");
		return -1;
	}

	values = PyObject_GetIter(values);
	if (!values)
	{
		PyErr_SetString(PyExc_Exception, "tree_items_source field is not an iterable");
        Py_DECREF(values);
		return -1;
	}

    new(&self->item_source_list) TArray<TSharedPtr<FPythonItem>>();
	while (PyObject *item = PyIter_Next(values))
	{
		Py_INCREF(item);
		// keep track of items
        self->item_source_list.Add(MakeShared<FPythonItem>(item));
	}
	arguments.TreeItemsSource(&self->item_source_list);

    {
        PyObject *value = ue_py_dict_get_item(kwargs, "header_row");
        if (value) {
            if (ue_PySHeaderRow *_py_swidget = py_ue_is_sheader_row(value)) {

                Py_INCREF(_py_swidget);
                arguments.HeaderRow(StaticCastSharedRef<SHeaderRow>(((ue_PySWidget *)_py_swidget)->Widget));
            }
            else {
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
    ue_py_slate_farguments_event("on_context_menu_opening", OnContextMenuOpening, FOnContextMenuOpening, OnContextMenuOpening);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_float("wheel_scroll_multiplier", WheelScrollMultiplier);
#endif
	ue_py_slate_farguments_event("on_get_children", OnGetChildren, TSlateDelegates<TSharedPtr<FPythonItem>>::FOnGetChildren, GetChildren);

	ue_py_snew(SPythonTreeView);
	return 0;
}

void ue_python_init_spython_tree_view(PyObject *ue_module)
{

	ue_PySPythonTreeViewType.tp_init = (initproc)ue_py_spython_tree_view_init;

	ue_PySPythonTreeViewType.tp_base = &ue_PySTreeViewType;

	if (PyType_Ready(&ue_PySPythonTreeViewType) < 0)
		return;

	Py_INCREF(&ue_PySPythonTreeViewType);
	PyModule_AddObject(ue_module, "SPythonTreeView", (PyObject *)&ue_PySPythonTreeViewType);
}
