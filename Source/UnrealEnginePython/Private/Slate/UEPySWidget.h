#pragma once

#include "UEPySlate.h"

#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"

#include "UEPyFInputEvent.h"
#include "UEPyFPointerEvent.h"
#include "UEPyFKeyEvent.h"
#include "UEPyFCharacterEvent.h"
#include "UEPyFModifierKeysState.h"

#define ue_py_slate_cast(T) ue_PySWidget *py_self_swidget = (ue_PySWidget *)self;\
	TSharedRef<T> py_ ## T = StaticCastSharedRef<T>(py_self_swidget->Widget)

#define ue_py_slate_cast_named(T, name) ue_PySWidget *py_self_swidget = (ue_PySWidget *)self;\
	TSharedRef<T> name = StaticCastSharedRef<T>(py_self_swidget->Widget)

#define Py_RETURN_SLATE_SELF Py_INCREF(self);\
	return (PyObject *)self

