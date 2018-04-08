#pragma once

#include "UnrealEnginePython.h"

#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"

#include "UEPyFInputEvent.h"
#include "UEPyFPointerEvent.h"
#include "UEPyFKeyEvent.h"
#include "UEPyFCharacterEvent.h"
#include "UEPyFModifierKeysState.h"

extern PyTypeObject ue_PySWidgetType;

typedef struct ue_PySWidget ue_PySWidget;

struct ue_PySWidget
{
	PyObject_HEAD
	/* Type-specific fields go here. */
	TSharedRef<SWidget> Widget;
	PyObject      *weakreflist;
};

void ue_python_init_swidget(PyObject *);

template<typename T>
TSharedPtr<T> py_ue_is_swidget(PyObject *py_obj)
{
	if (!PyObject_IsInstance(py_obj, (PyObject *)&ue_PySWidgetType))
	{
		PyErr_SetString(PyExc_Exception, "object is not an SWidget");
		return nullptr;
	}
	return StaticCastSharedRef<T>(((ue_PySWidget *)py_obj)->Widget);
}

#define ue_py_slate_cast(T) ue_PySWidget *py_self_swidget = (ue_PySWidget *)self;\
	TSharedRef<T> py_ ## T = StaticCastSharedRef<T>(py_self_swidget->Widget)

#define ue_py_slate_cast_named(T, name) ue_PySWidget *py_self_swidget = (ue_PySWidget *)self;\
	TSharedRef<T> name = StaticCastSharedRef<T>(py_self_swidget->Widget)

#define Py_RETURN_SLATE_SELF Py_INCREF(self);\
	return (PyObject *)self

