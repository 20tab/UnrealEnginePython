#include "UnrealEnginePythonPrivatePCH.h"


#include "Runtime/UMG/Public/Components/Widget.h"


PyObject *py_ue_take_widget(ue_PyUObject * self, PyObject * args) {

	ue_py_check(self);

    UWidget *widget = ue_py_check_type<UWidget>(self);
	if (!widget)
		return PyErr_Format(PyExc_Exception, "uobject is not a UWidget");

	ue_PySWidget *s_widget = ue_py_get_swidget(widget->TakeWidget());

    return (PyObject *)s_widget;
}
