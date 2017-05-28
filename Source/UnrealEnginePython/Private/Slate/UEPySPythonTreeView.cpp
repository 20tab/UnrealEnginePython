
#include "UnrealEnginePythonPrivatePCH.h"

#include "Runtime/Slate/Public/Widgets/Views/STreeView.h"

#include "UEPySPythonTreeView.h"


#define GET_s_python_tree_view SPythonTreeView *s_python_tree_view = (SPythonTreeView *)self->s_tree_view.s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget


static PyMethodDef ue_PySPythonTreeView_methods[] = {
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PySPythonTreeView_str(ue_PySPythonTreeView *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SPythonTreeView '%p'>",
		self->s_tree_view.s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget);
}

PyTypeObject ue_PySPythonTreeViewType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonTreeView", /* tp_name */
	sizeof(ue_PySPythonTreeView), /* tp_basicsize */
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
	(reprfunc)ue_PySPythonTreeView_str,                         /* tp_str */
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

static int ue_py_spython_tree_view_init(ue_PySPythonTreeView *self, PyObject *args, PyObject *kwargs) {
	PyObject *py_iterable;
	PyObject *py_callable;
	PyObject *py_callable_children;
	if (!PyArg_ParseTuple(args, "OOO", &py_iterable, &py_callable, &py_callable_children)) {
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

	if (!PyCallable_Check(py_callable_children)) {
		PyErr_SetString(PyExc_Exception, "argument is not a callable");
		return -1;
	}

	// this will be destroyed by the SPythonListView
	auto items = new TArray<TSharedPtr<FPythonItem>>();

	while (PyObject *item = PyIter_Next(py_iterable)) {
		Py_INCREF(item);
		items->Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}

	Py_DECREF(py_iterable);

	SPythonTreeView::FOnGenerateRow handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::GenerateRow);

	SPythonTreeView::FOnGetChildren handler_children;
	UPythonSlateDelegate *py_delegate_children = NewObject<UPythonSlateDelegate>();
	py_delegate_children->SetPyCallable(py_callable_children);
	py_delegate_children->AddToRoot();
	handler_children.BindUObject(py_delegate_children, &UPythonSlateDelegate::GetChildren);

	new(&self->s_tree_view.s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget_owned) TSharedRef<SWidget>(SNew(SPythonTreeView).TreeItemsSource(items).OnGenerateRow(handler).OnGetChildren(handler_children));
	self->s_tree_view.s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget = &self->s_tree_view.s_list_view.s_table_view_base.s_compound_widget.s_widget.s_widget_owned.Get();
	return 0;
}

void ue_python_init_spython_tree_view(PyObject *ue_module) {
	ue_PySPythonTreeViewType.tp_new = PyType_GenericNew;

	ue_PySPythonTreeViewType.tp_init = (initproc)ue_py_spython_tree_view_init;

	ue_PySPythonTreeViewType.tp_base = &ue_PySTreeViewType;

	if (PyType_Ready(&ue_PySPythonTreeViewType) < 0)
		return;

	Py_INCREF(&ue_PySTreeViewType);
	PyModule_AddObject(ue_module, "SPythonTreeView", (PyObject *)&ue_PySPythonTreeViewType);
}
