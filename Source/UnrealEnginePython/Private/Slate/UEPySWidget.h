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

#define DECLARE_UE_PY_SLATE_WIDGET(WIDGETTYPE) PyTypeObject ue_Py##WIDGETTYPE##Type = {\
PyVarObject_HEAD_INIT(NULL, 0)\
"unreal_engine." #WIDGETTYPE, /* tp_name */\
sizeof(ue_Py##WIDGETTYPE), /* tp_basicsize */\
0,                         /* tp_itemsize */\
0,							/* tp_dealloc */\
0,                         /* tp_print */\
0,                         /* tp_getattr */\
0,                         /* tp_setattr */\
0,                         /* tp_reserved */\
0,                         /* tp_repr */\
0,                         /* tp_as_number */\
0,                         /* tp_as_sequence */\
0,                         /* tp_as_mapping */\
0,                         /* tp_hash  */\
0,                         /* tp_call */\
0,                         /* tp_str */\
0,                         /* tp_getattro */\
0,                         /* tp_setattro */\
0,                         /* tp_as_buffer */\
Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */\
"Unreal Engine " #WIDGETTYPE,           /* tp_doc */\
0,                         /* tp_traverse */\
0,                         /* tp_clear */\
0,                         /* tp_richcompare */\
0,                         /* tp_weaklistoffset */\
0,                         /* tp_iter */\
0,                         /* tp_iternext */\
ue_Py##WIDGETTYPE##_methods,             /* tp_methods */\
}

