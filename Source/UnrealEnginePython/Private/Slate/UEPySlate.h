#pragma once

#include "UEPyModule.h"

#include "Runtime/SlateCore/Public/Widgets/DeclarativeSyntaxSupport.h"


#if WITH_EDITOR
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "UnrealEdMisc.h"
#endif



#include "UEPySlateDelegate.h"
#include "UEPySlatePythonItem.h"

void ue_python_init_swidget(PyObject *);

extern PyTypeObject ue_PySWidgetType;

typedef struct ue_PySWidget ue_PySWidget;

struct ue_PySWidget
{
	PyObject_HEAD
		/* Type-specific fields go here. */
		TSharedRef<SWidget> Widget;
	PyObject      *weakreflist;
};

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



PyObject *py_unreal_engine_get_editor_window(PyObject *, PyObject *);

PyObject *py_unreal_engine_find_slate_style(PyObject *, PyObject *);
PyObject *py_unreal_engine_find_icon_for_class(PyObject *, PyObject *);

#if WITH_EDITOR
PyObject *py_unreal_engine_add_menu_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_menu_bar_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_add_tool_bar_extension(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_detail_view(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_create_structure_detail_view(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_create_property_view(PyObject *, PyObject *, PyObject *);

PyObject *py_unreal_engine_add_asset_view_context_menu_extension(PyObject * self, PyObject *);
#endif

PyObject *py_unreal_engine_invoke_tab(PyObject *, PyObject *);
PyObject *py_unreal_engine_get_swidget_from_wrapper(PyObject *, PyObject *);
PyObject *py_unreal_engine_create_wrapper_from_pyswidget(PyObject *, PyObject *);

PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject *, PyObject *);
PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject *, PyObject *);

PyObject *py_unreal_engine_open_color_picker(PyObject *, PyObject *, PyObject *);
PyObject *py_unreal_engine_destroy_color_picker(PyObject *, PyObject *);

PyObject *py_unreal_engine_play_sound(PyObject *, PyObject *);

PyObject *ue_py_dict_get_item(PyObject *, const char *);

template<typename T> ue_PySWidget *py_ue_new_swidget(TSharedRef<SWidget> s_widget, PyTypeObject *py_type)
{
	ue_PySWidget *ret = (ue_PySWidget *)PyObject_New(T, py_type);

	new(&ret->Widget) TSharedRef<SWidget>(s_widget);
	ret->weakreflist = nullptr;
	return ret;
}

#define ue_py_snew_base(T, required, arguments) ((ue_PySWidget *)self)->Widget = TSharedRef<T>(MakeTDecl<T>(#T, __FILE__, __LINE__, required) <<= arguments);\
				for(TSharedRef<FPythonSlateDelegate> Delegate : DeferredSlateDelegates)\
				{\
					FUnrealEnginePythonHouseKeeper::Get()->TrackDeferredSlateDelegate(Delegate, ((ue_PySWidget *)self)->Widget);\
				}

#define ue_py_snew_simple(T) TArray<TSharedRef<FPythonSlateDelegate>> DeferredSlateDelegates;\
	ue_py_snew_base(T, RequiredArgs::MakeRequiredArgs(), T::FArguments())

#define ue_py_snew_simple_with_req_args(T, ... ) TArray<TSharedRef<FPythonSlateDelegate>> DeferredSlateDelegates;\
	ue_py_snew_base(T, RequiredArgs::MakeRequiredArgs(__VA_ARGS__), T::FArguments())

#define ue_py_snew(T) ue_py_snew_base(T, RequiredArgs::MakeRequiredArgs(), arguments)

#define ue_py_snew_with_args(T, args) ue_py_snew_base(T, RequiredArgs::MakeRequiredArgs(args), arguments)


ue_PySWidget *ue_py_get_swidget(TSharedRef<SWidget> s_widget);

#define ue_py_slate_base_up(_base, _func, _param, _attribute) \
{\
	PyObject *value = ue_py_dict_get_item(kwargs, _param);\
	if (value) {\
		if (PyCallable_Check(value)) {\
			_base handler;\
			TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDeferredSlateDelegate(value);\
			handler.Bind(py_delegate, &FPythonSlateDelegate::_func);\
			DeferredSlateDelegates.Add(py_delegate);\
			arguments._attribute(handler);\
		}

#define ue_py_slate_base_event_up(_base, _func, _param, _attribute) \
{\
	PyObject *value = ue_py_dict_get_item(kwargs, _param);\
	if (value) {\
		if (PyCallable_Check(value)) {\
			_base handler;\
			TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewDeferredSlateDelegate(value);\
			handler.BindSP(py_delegate, &FPythonSlateDelegate::_func);\
			DeferredSlateDelegates.Add(py_delegate);\
			arguments._attribute(handler);\
		}

#define ue_py_slate_down(_param) else {\
			PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " _param); \
			return -1;\
		}\
	}\
}

#define ue_py_slate_up(_type, _func, _param, _attribute) ue_py_slate_base_up(TAttribute<_type>, _func, _param, _attribute)

#define ue_py_slate_farguments_text(param, attribute) ue_py_slate_up(FText, GetterFText, param, attribute)\
	else if (PyUnicodeOrString_Check(value)) {\
		arguments.attribute(FText::FromString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(value))));\
	}\
	ue_py_slate_down(param)


#define ue_py_slate_farguments_string(param, attribute) ue_py_slate_up(FString, GetterFString, param, attribute)\
	else if (PyUnicodeOrString_Check(value)) {\
		arguments.attribute(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(value)));\
	}\
	ue_py_slate_down(param)



#define ue_py_slate_farguments_float(param, attribute) ue_py_slate_up(float, GetterFloat, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_tfloat(param, attribute) ue_py_slate_up(TOptional<float>, GetterTFloat, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute((TOptional<float>)PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_fvector2d(param, attribute) ue_py_slate_up(FVector2D, GetterFVector2D, param, attribute)\
		else if (PyTuple_Check(value)) {\
			if (PyTuple_Size(value) == 2) {\
				PyObject *py_first = PyTuple_GetItem(value, 0);\
				PyObject *py_second = PyTuple_GetItem(value, 1);\
				if (PyNumber_Check(py_first)) {\
					PyObject *py_x = PyNumber_Float(py_first);\
					PyObject *py_y = PyNumber_Float(py_second);\
					arguments.attribute(FVector2D(PyFloat_AsDouble(py_x), PyFloat_AsDouble(py_y)));\
					Py_DECREF(py_x);\
					Py_DECREF(py_y);\
				}\
			}\
		}\
		ue_py_slate_down(param)





#define ue_py_slate_farguments_int(param, attribute) ue_py_slate_up(int, GetterInt, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((int)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_int32(param, attribute) ue_py_slate_up(int32, GetterInt, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((int32)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_tint(param, attribute) ue_py_slate_up(TOptional<int32>, GetterIntT<TOptional<int32>>, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((TOptional<int32>)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)


#define ue_py_slate_farguments_enum(param, attribute, _type) ue_py_slate_up(_type, GetterIntT<_type>, param, attribute)\
		else if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((_type)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_flinear_color(param, attribute) ue_py_slate_up(FLinearColor, GetterFLinearColor, param, attribute)\
		else if (ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(value)) {\
			arguments.attribute(py_color->color); \
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_struct(param, attribute, _type) ue_py_slate_up(_type, GetterStructT<_type>, param, attribute)\
		else if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
			arguments.attribute((_type)*u_struct); \
		}\
		ue_py_slate_down(param)



#define ue_py_slate_farguments_optional_struct(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
				arguments.attribute((_type)*u_struct); \
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_struct_ptr(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_struct = ue_py_check_struct<_type>(value)) {\
				Py_INCREF(value);\
				arguments.attribute((_type *)u_struct);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}

#define ue_py_slate_farguments_optional_uobject(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (_type *u_object = ue_py_check_type<_type>(value)) {\
				arguments.attribute((_type *)u_object);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_fvector2d(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (PyTuple_Check(value)) {\
				if (PyTuple_Size(value) == 2) {\
					PyObject *py_first = PyTuple_GetItem(value, 0);\
					PyObject *py_second = PyTuple_GetItem(value, 1);\
					if (PyNumber_Check(py_first)) {\
						PyObject *py_x = PyNumber_Float(py_first);\
						PyObject *py_y = PyNumber_Float(py_second);\
						arguments.attribute(FVector2D(PyFloat_AsDouble(py_x), PyFloat_AsDouble(py_y)));\
						Py_DECREF(py_x);\
						Py_DECREF(py_y);\
					}\
				}\
				else {\
					PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
					return -1;\
				}\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_enum(param, attribute, _type) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
		if (value) {\
			if (PyNumber_Check(value)) {\
				PyObject *py_int = PyNumber_Long(value);\
				arguments.attribute((_type)PyLong_AsLong(py_int));\
				Py_DECREF(py_int);\
			}\
			else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
			}\
		}\
}


#define ue_py_slate_farguments_optional_int32(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyNumber_Check(value)) {\
			PyObject *py_int = PyNumber_Long(value);\
			arguments.attribute((int32)PyLong_AsLong(py_int)); \
			Py_DECREF(py_int);\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}


#define ue_py_slate_farguments_optional_float(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float);\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_foptional_size(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyNumber_Check(value)) {\
			PyObject *py_float = PyNumber_Float(value);\
			arguments.attribute(FOptionalSize(PyFloat_AsDouble(py_float))); \
			Py_DECREF(py_float);\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_string(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (PyUnicodeOrString_Check(value)) {\
		arguments.attribute(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(value)));\
	}\
}

#define ue_py_slate_farguments_optional_text(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyUnicodeOrString_Check(value)) {\
			arguments.attribute(FText::FromString(UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(value))));\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}

#define ue_py_slate_farguments_optional_named_slot(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		TSharedPtr<SWidget> Child = py_ue_is_swidget<SWidget>(value);\
		if (Child.IsValid()) {\
            arguments.attribute()\
            [\
                Child.ToSharedRef()\
            ];\
		}\
		else {\
				PyErr_SetString(PyExc_TypeError, "unsupported type for attribute " param); \
				return -1;\
		}\
	}\
}


#define ue_py_slate_farguments_bool(param, attribute) ue_py_slate_up(bool, GetterBool, param, attribute)\
		else if (PyObject_IsTrue(value)) {\
			arguments.attribute(true); \
		}\
		else {\
			arguments.attribute(false); \
		}\
	}\
}

#define ue_py_slate_farguments_call(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value && PyObject_IsTrue(value)) {\
		arguments.attribute();\
	}\
}

#define ue_py_slate_farguments_padding(param, attribute) { PyObject *padding = ue_py_dict_get_item(kwargs, param);\
	if (padding)\
	{\
		if (PyTuple_Check(padding))\
		{\
			FMargin margin;\
			if (!PyArg_ParseTuple(padding, "f|fff", &margin.Left, &margin.Top, &margin.Right, &margin.Bottom))\
			{\
				PyErr_SetString(PyExc_TypeError, "invalid padding value");\
				return -1;\
			}\
			arguments.attribute(margin);\
		}\
		else if (PyNumber_Check(padding))\
		{\
			PyObject *py_float = PyNumber_Float(padding); \
			arguments.attribute(PyFloat_AsDouble(py_float)); \
			Py_DECREF(py_float); \
		}\
		else if (FMargin *u_struct = ue_py_check_struct<FMargin>(padding))\
		{\
			arguments.attribute(*u_struct); \
		}\
		else\
		{\
			PyErr_SetString(PyExc_TypeError, "invalid padding value"); \
			return -1; \
		}\
	}\
}


#define ue_py_slate_farguments_optional_bool(param, attribute) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
	if (value) {\
		if (PyObject_IsTrue(value)) {\
			arguments.attribute(true); \
		}\
		else {\
			arguments.attribute(false); \
		}\
	}\
}



#define ue_py_slate_farguments_event(param, attribute, type, method) ue_py_slate_base_event_up(type, method, param, attribute)\
		ue_py_slate_down(param)



#define ue_py_slate_setup_farguments(_type) _type::FArguments arguments;\
	TArray<TSharedRef<FPythonSlateDelegate>> DeferredSlateDelegates;\
	ue_py_slate_farguments_bool("is_enabled", IsEnabled); \
	ue_py_slate_farguments_text("tool_tip_text", ToolTipText); \
	ue_py_slate_farguments_fvector2d("render_transform_pivot", RenderTransformPivot)

#define ue_py_slate_farguments_required_slot(param) { PyObject *value = ue_py_dict_get_item(kwargs, param);\
    value = value ? value : PyTuple_GetItem(args, 0);\
    if (!value) {PyErr_Clear(); PyErr_SetString(PyExc_TypeError, "you need to specify a widget"); return -1;}\
	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(value);\
	if (Widget.IsValid())\
		arguments.AttachWidget(Widget.ToSharedRef());\
	else\
	{\
		PyErr_SetString(PyExc_TypeError, "unsupported type for required slot " param);\
		return -1;\
	}\
}

#define ue_py_slate_setup_hack_slot_args(_type, _swidget_ref) _type::FSlot &arguments = _swidget_ref->AddSlot();\
	TArray<TSharedRef<FPythonSlateDelegate>> DeferredSlateDelegates;\
    ue_py_slate_farguments_required_slot("widget");

#define ue_py_slate_setup_hack_slot_args_grid(_type, _swidget_ref, column, row, layer) _type::FSlot &arguments = _swidget_ref->AddSlot(column, row, layer);\
	TArray<TSharedRef<FPythonSlateDelegate>> DeferredSlateDelegates;\
    ue_py_slate_farguments_required_slot("widget");

void ue_python_init_slate(PyObject *);




