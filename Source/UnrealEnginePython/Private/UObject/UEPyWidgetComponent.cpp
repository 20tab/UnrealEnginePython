#include "UnrealEnginePythonPrivatePCH.h"


#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "PyUserWidget.h"


PyObject *py_ue_set_slate_widget(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_widget;

	if (!PyArg_ParseTuple(args, "O", &py_widget))
	{
		return nullptr;
	}

	UWidgetComponent *widget_component = ue_py_check_type<UWidgetComponent>(self);
    UPyUserWidget *py_user_widget      = ue_py_check_type<UPyUserWidget>(self);
	if (!widget_component && !py_user_widget)
		return PyErr_Format(PyExc_Exception, "uobject is not a UWidgetComponent or UPyUserWidget");

	ue_PySWidget *s_widget = py_ue_is_swidget(py_widget);
	if (!s_widget)
		return PyErr_Format(PyExc_Exception, "argument is not a SWidget");

         if (widget_component) { widget_component->SetSlateWidget(s_widget->s_widget); }
    else                       { py_user_widget->SetSlateWidget(s_widget->s_widget);   }

	Py_RETURN_NONE;
}
