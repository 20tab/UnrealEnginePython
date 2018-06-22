#include "UEPyWidget.h"


#include "Runtime/UMG/Public/Components/Widget.h"
#include "Slate/UEPySWidget.h"


PyObject *py_ue_take_widget(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	UWidget *widget = ue_py_check_type<UWidget>(self);
	if (!widget)
		return PyErr_Format(PyExc_Exception, "uobject is not a UWidget");

	ue_PySWidget *s_widget = ue_py_get_swidget(widget->TakeWidget());

	return (PyObject *)s_widget;
}

PyObject *py_ue_create_widget(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_class;
	if (!PyArg_ParseTuple(args, "O", &py_class))
		return nullptr;

	APlayerController *player = ue_py_check_type<APlayerController>(self);
	if (!player)
		return PyErr_Format(PyExc_Exception, "uobject is not a APlayerController");

	UClass *u_class = ue_py_check_type<UClass>(py_class);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass");

	if (!u_class->IsChildOf<UUserWidget>())
		return PyErr_Format(PyExc_Exception, "argument is not a child of UUserWidget");

	UUserWidget *widget = CreateWidget<UUserWidget>(player, u_class);
	if (!widget)
		return PyErr_Format(PyExc_Exception, "unable to create new widget");

	Py_RETURN_UOBJECT(widget);
}
