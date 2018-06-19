#include "UEPyFToolBarBuilder.h"

#include "UEPyFSlateIcon.h"
#include "Runtime/Slate/Public/Framework/Commands/UIAction.h"

static PyObject *py_ue_ftool_bar_builder_begin_section(ue_PyFToolBarBuilder *self, PyObject * args)
{
	char *name;
	if (!PyArg_ParseTuple(args, "s:begin_section", &name))
		return NULL;

	self->tool_bar_builder.BeginSection(FName(name));

	Py_RETURN_NONE;
}

static PyObject *py_ue_ftool_bar_builder_end_section(ue_PyFToolBarBuilder *self, PyObject * args)
{
	self->tool_bar_builder.EndSection();

	Py_RETURN_NONE;
}

static PyObject *py_ue_ftool_bar_builder_add_tool_bar_button(ue_PyFToolBarBuilder *self, PyObject * args)
{
	char *hook;
	char *label;
	char *tooltip;
	PyObject *py_icon;
	PyObject *py_callable;
	PyObject *py_obj = nullptr;
	if (!PyArg_ParseTuple(args, "sssOO|O:add_tool_bar_button", &hook, &label, &tooltip, &py_icon, &py_callable, &py_obj))
		return NULL;

	ue_PyFSlateIcon *py_slate_icon = py_ue_is_fslate_icon(py_icon);
	if (!py_slate_icon)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateIcon");
	}

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

	self->tool_bar_builder.AddToolBarButton(FUIAction(handler), FName(hook), FText::FromString(UTF8_TO_TCHAR(label)), FText::FromString(UTF8_TO_TCHAR(tooltip)), py_slate_icon->icon);

	Py_RETURN_NONE;
}

static PyObject *py_ue_ftool_bar_builder_add_separator(ue_PyFToolBarBuilder *self, PyObject * args)
{
	char *hook = nullptr;

	if (!PyArg_ParseTuple(args, "|s:add_separator", &hook))
		return NULL;

	FName f_name = NAME_None;

	if (hook)
		f_name = FName(UTF8_TO_TCHAR(hook));

	self->tool_bar_builder.AddSeparator(f_name);

	Py_RETURN_NONE;
}


static PyObject *py_ue_ftool_bar_builder_begin_block_group(ue_PyFToolBarBuilder *self, PyObject * args)
{
	self->tool_bar_builder.BeginBlockGroup();

	Py_RETURN_NONE;
}

static PyObject *py_ue_ftool_bar_builder_end_block_group(ue_PyFToolBarBuilder *self, PyObject * args)
{
	self->tool_bar_builder.EndBlockGroup();

	Py_RETURN_NONE;
}

static PyObject *py_ue_ftool_bar_builder_make_widget(ue_PyFToolBarBuilder *self, PyObject * args)
{
	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(self->tool_bar_builder.MakeWidget(), &ue_PySWidgetType);
}

static PyMethodDef ue_PyFToolBarBuilder_methods[] = {
	{ "begin_section", (PyCFunction)py_ue_ftool_bar_builder_begin_section, METH_VARARGS, "" },
	{ "end_section", (PyCFunction)py_ue_ftool_bar_builder_end_section, METH_VARARGS, "" },
	{ "add_tool_bar_button", (PyCFunction)py_ue_ftool_bar_builder_add_tool_bar_button, METH_VARARGS, "" },
	{ "add_separator", (PyCFunction)py_ue_ftool_bar_builder_add_separator, METH_VARARGS, "" },
	{ "begin_block_group", (PyCFunction)py_ue_ftool_bar_builder_begin_block_group, METH_VARARGS, "" },
	{ "end_block_group", (PyCFunction)py_ue_ftool_bar_builder_end_block_group, METH_VARARGS, "" },
	{ "make_widget", (PyCFunction)py_ue_ftool_bar_builder_make_widget, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFToolBarBuilder_str(ue_PyFToolBarBuilder *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FToolBarBuilder '%p'}>",
		&self->tool_bar_builder);
}

static void ue_py_ftool_bar_builder_dealloc(ue_PyFToolBarBuilder *self)
{
#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyTypeObject ue_PyFToolBarBuilderType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FToolBarBuilder", /* tp_name */
	sizeof(ue_PyFToolBarBuilder), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_ftool_bar_builder_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyFToolBarBuilder_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FToolBarBuilder",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFToolBarBuilder_methods,             /* tp_methods */
};

static int ue_py_ftool_bar_builder_init(ue_PyFToolBarBuilder *self, PyObject *args, PyObject *kwargs)
{
	new(&self->tool_bar_builder) FToolBarBuilder(TSharedPtr<FUICommandList>(), FMultiBoxCustomization::None);
	return 0;
}


void ue_python_init_ftool_bar_builder(PyObject *ue_module)
{
	ue_PyFToolBarBuilderType.tp_new = PyType_GenericNew;

	ue_PyFToolBarBuilderType.tp_init = (initproc)ue_py_ftool_bar_builder_init;

	if (PyType_Ready(&ue_PyFToolBarBuilderType) < 0)
		return;

	Py_INCREF(&ue_PyFToolBarBuilderType);
	PyModule_AddObject(ue_module, "FToolBarBuilder", (PyObject *)&ue_PyFToolBarBuilderType);
}

PyObject *py_ue_new_ftool_bar_builder(FToolBarBuilder tool_bar_builder)
{
	ue_PyFToolBarBuilder *ret = (ue_PyFToolBarBuilder *)PyObject_New(ue_PyFToolBarBuilder, &ue_PyFToolBarBuilderType);
	new(&ret->tool_bar_builder) FToolBarBuilder(tool_bar_builder);
	return (PyObject *)ret;
}
