

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySButton.h"



#define GET_s_button SButton *s_button =(SButton *)self->s_border.s_compound_widget.s_widget.s_widget

static PyObject *py_ue_sbutton_is_pressed(ue_PySButton *self, PyObject * args) {
	GET_s_button;

	if (s_button->IsPressed()) {
		Py_INCREF(Py_True);
		return Py_True;
	}

	Py_INCREF(Py_False);
	return Py_False;
}


static PyObject *ue_PySButton_str(ue_PySButton *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SButton '%p'>",
		self->s_border.s_compound_widget.s_widget.s_widget);
}

static PyObject *py_ue_sbutton_bind_on_clicked(ue_PySButton *self, PyObject * args) {
	PyObject *py_callable;
	if (!PyArg_ParseTuple(args, "O:bind_on_clicked", &py_callable)) {
		return NULL;
	}

	if (!PyCallable_Check(py_callable)) {
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	FOnClicked handler;
	UPythonSlateDelegate *py_delegate = NewObject<UPythonSlateDelegate>();
	py_delegate->SetPyCallable(py_callable);
	py_delegate->AddToRoot();
	handler.BindUObject(py_delegate, &UPythonSlateDelegate::OnClicked);

	GET_s_button;

	s_button->SetOnClicked(handler);

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySButton_methods[] = {
	{ "is_pressed", (PyCFunction)py_ue_sbutton_is_pressed, METH_VARARGS, "" },
	{ "bind_on_clicked", (PyCFunction)py_ue_sbutton_bind_on_clicked, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySButtonType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SButton", /* tp_name */
	sizeof(ue_PySButton), /* tp_basicsize */
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
	(reprfunc)ue_PySButton_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SButton",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySButton_methods,             /* tp_methods */
};

static int ue_py_sbutton_init(ue_PySButton *self, PyObject *args, PyObject *kwargs) {
	ue_py_snew(SButton, s_border.s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sbutton(PyObject *ue_module) {
	ue_PySButtonType.tp_new = PyType_GenericNew;

	ue_PySButtonType.tp_init = (initproc)ue_py_sbutton_init;

	ue_PySButtonType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySButtonType) < 0)
		return;

	Py_INCREF(&ue_PySButtonType);
	PyModule_AddObject(ue_module, "SButton", (PyObject *)&ue_PySButtonType);
}
