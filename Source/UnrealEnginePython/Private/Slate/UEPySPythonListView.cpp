
#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/Slate/Public/Widgets/Views/SListView.h"

#include "UEPySPythonListView.h"


#define GET_s_python_list_view SPythonListView *s_python_list_view = (SPythonListView *)self->s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget

/*static PyObject *py_ue_spython_list_view_is_item_visible(ue_PySPythonListView *self, PyObject * args) {
	PyObject *py_item;
	if (!PyArg_ParseTuple(args, "O:is_item_visible", &py_item)) {
		return NULL;
	}

	GET_s_python_list_view;

	if (s_python_list_view->IsItemVisible(TSharedPtr<FPythonItem>(py_item))) {
		Py_INCREF(Py_True);
		return Py_True;
	}
	else {
		Py_INCREF(Py_False);
		return Py_False;
	}
}*/

static PyObject *ue_PySPythonListView_str(ue_PySPythonListView *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SPythonListView '%p'>",
		self->s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget);
}

static PyMethodDef ue_PySPythonListView_methods[] = {
	//{ "is_item_visible", (PyCFunction)py_ue_spython_list_view_is_item_visible, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySPythonListViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonListView", /* tp_name */
	sizeof(ue_PySPythonListView), /* tp_basicsize */
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
	(reprfunc)ue_PySPythonListView_str,                         /* tp_str */
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

static int ue_py_spython_list_view_init(ue_PySPythonListView *self, PyObject *args, PyObject *kwargs) {
	PyObject *py_iterable;
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "OO", &py_iterable, &py_callable)) {
		return -1;
	}

	py_iterable = PyObject_GetIter(py_iterable);

	if (!py_iterable || !PyIter_Check(py_iterable)) {
		PyErr_SetString(PyExc_Exception, "argument is not an iterable");
		return -1;
	}

	if (!PyCallable_Check(py_callable)) {
		PyErr_SetString(PyExc_Exception, "argument is not a callable");
		return -1;
	}

	auto items = new TArray<TSharedPtr<FPythonItem>>();

	while (PyObject *item = PyIter_Next(py_iterable)) {
		Py_INCREF(item);
		items->Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}

	Py_DECREF(py_iterable);

	SPythonListView::FOnGenerateRow handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::GenerateWidgetForList);

	self->s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget_owned = SNew(SPythonListView).ListItemsSource(items).OnGenerateRow(handler);
	self->s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget = &self->s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget_owned.Get();
	return 0;
}

void ue_python_init_spython_list_view(PyObject *ue_module) {
	ue_PySPythonListViewType.tp_new = PyType_GenericNew;

	ue_PySPythonListViewType.tp_init = (initproc)ue_py_spython_list_view_init;

	ue_PySPythonListViewType.tp_base = &ue_PySListViewType;

	if (PyType_Ready(&ue_PySPythonListViewType) < 0)
		return;

	Py_INCREF(&ue_PySListViewType);
	PyModule_AddObject(ue_module, "SPythonListView", (PyObject *)&ue_PySPythonListViewType);
}
