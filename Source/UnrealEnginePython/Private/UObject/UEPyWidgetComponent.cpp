#include "UEPyWidgetComponent.h"


#include "Runtime/UMG/Public/Components/WidgetComponent.h"
#include "PyUserWidget.h"
#include "Slate/UEPySWidget.h"


PyObject *py_ue_set_slate_widget(ue_PyUObject * self, PyObject * args)
{

	ue_py_check(self);

	PyObject *py_widget;

	if (!PyArg_ParseTuple(args, "O", &py_widget))
	{
		return nullptr;
	}

	UWidgetComponent *widget_component = ue_py_check_type<UWidgetComponent>(self);
	UPyUserWidget *py_user_widget = ue_py_check_type<UPyUserWidget>(self);
	if (!widget_component && !py_user_widget)
		return PyErr_Format(PyExc_Exception, "uobject is not a UWidgetComponent or UPyUserWidget");

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(py_widget);
	if (!Widget.IsValid())
		return nullptr;

	if (widget_component) { widget_component->SetSlateWidget(Widget); }
	else { py_user_widget->SetSlateWidget(Widget.ToSharedRef()); }

	Py_RETURN_NONE;
}
