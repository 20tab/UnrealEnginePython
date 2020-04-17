
#include "UEPyFSlateStyleSet.h"
#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyleRegistry.h"
#include "Runtime/SlateCore/Public/Sound/SlateSound.h"
#include "Runtime/Core/Public/Containers/Map.h"

static PyObject *py_ue_fslate_style_set_set_content_root(ue_PyFSlateStyleSet *self, PyObject * args)
{
	char *path;
	if (!PyArg_ParseTuple(args, "s:set_content_root", &path))
		return NULL;

	self->style_set->SetContentRoot(path);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslate_style_set_register(ue_PyFSlateStyleSet *self, PyObject * args)
{

	if (FSlateStyleRegistry::FindSlateStyle(self->style_set->GetStyleSetName()))
	{
		UE_LOG(LogPython, Warning, TEXT("StyleSet already registered"));
		Py_RETURN_NONE;
	}
	FSlateStyleRegistry::RegisterSlateStyle(*self->style_set);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fslate_style_set_set(ue_PyFSlateStyleSet *self, PyObject * args)
{
	char *name;
	PyObject *py_value;
	if (!PyArg_ParseTuple(args, "sO:set", &name, &py_value))
		return NULL;

	FSlateSound *slate_sound = ue_py_check_struct<FSlateSound>(py_value);
	if (slate_sound)
	{
		self->style_set->Set(FName(name), *slate_sound);
		Py_RETURN_NONE;
	}

	FSlateBrush *slate_brush = ue_py_check_struct<FSlateBrush>(py_value);
	if (slate_brush)
	{
		self->style_set->Set(FName(name), slate_brush);
		Py_RETURN_NONE;
	}

	FSlateColor *slate_color = ue_py_check_struct<FSlateColor>(py_value);
	if (slate_brush)
	{
		self->style_set->Set(FName(name), *slate_color);
		Py_RETURN_NONE;
	}

	FSlateFontInfo *slate_font = ue_py_check_struct<FSlateFontInfo>(py_value);
	if (slate_font)
	{
		self->style_set->Set(FName(name), *slate_font);
		Py_RETURN_NONE;
	}

	ue_PyFLinearColor *py_linear_color = py_ue_is_flinearcolor(py_value);
	if (py_linear_color)
	{
		self->style_set->Set(FName(name), py_linear_color->color);
		Py_RETURN_NONE;
	}

	ue_PyFColor *py_color = py_ue_is_fcolor(py_value);
	if (py_color)
	{
		self->style_set->Set(FName(name), py_color->color);
		Py_RETURN_NONE;
	}

	if (PyNumber_Check(py_value))
	{
		PyObject *py_float = PyNumber_Float(py_value);
		self->style_set->Set(FName(name), (float)PyFloat_AsDouble(py_float));
		Py_DECREF(py_float);
		Py_RETURN_NONE;
	}

	return PyErr_Format(PyExc_ValueError, "unsupported value type");
}

namespace
{
	template <typename WidgetStyleType>
	PyObject* pyGetWidgetStyle(FSlateStyleSet& InStyle, FName PropertyName)
	{
		const WidgetStyleType styleWidgetStyle = InStyle.GetWidgetStyle<WidgetStyleType>(PropertyName);
		return py_ue_new_owned_uscriptstruct(WidgetStyleType::StaticStruct(), (uint8*)&styleWidgetStyle);
	}
}

static PyObject *py_ue_fslate_style_set_get(ue_PyFSlateStyleSet *self, PyObject * args)
{
	if (!(self && self->style_set))
		return PyErr_Format(PyExc_Exception, "fstyleset is in invalid state");

	char *name = nullptr;
	PyObject *py_type = nullptr;

	if (!PyArg_ParseTuple(args, "sO:get", &name, &py_type))
		return NULL;

	PyObject *ret = nullptr;

	if (ue_py_check_struct<FSlateSound>(py_type))
	{
		const FSlateSound& styleSound = self->style_set->GetSound(FName(name));
		ret = py_ue_new_owned_uscriptstruct(FSlateSound::StaticStruct(), (uint8*)&styleSound);
	}
	else if (ue_py_check_struct<FSlateBrush>(py_type))
	{
		if (const FSlateBrush* styleBrush = self->style_set->GetBrush(FName(name)))
		{
			ret = py_ue_new_owned_uscriptstruct(FSlateBrush::StaticStruct(), (uint8*)styleBrush);
		}
	}
	else if (ue_py_check_struct<FSlateColor>(py_type))
	{
		const FSlateColor styleSlateColor = self->style_set->GetSlateColor(FName(name));
		ret = py_ue_new_owned_uscriptstruct(FSlateColor::StaticStruct(), (uint8*)&styleSlateColor);
	}
	else if (ue_py_check_struct<FSlateFontInfo>(py_type))
	{
		const FSlateFontInfo styleFontInfo = self->style_set->GetFontStyle(FName(name));
		ret = py_ue_new_owned_uscriptstruct(FSlateFontInfo::StaticStruct(), (uint8*)&styleFontInfo);
	}
	else if (ue_py_check_childstruct<FSlateWidgetStyle>(py_type))
	{

		typedef TFunction<PyObject* (FSlateStyleSet&, FName)> WStyleGetter;
		typedef TPair<UScriptStruct*, WStyleGetter> WStylePair;
#if ENGINE_MINOR_VERSION > 15
		static const WStylePair validWidgetStyleUStructList[] = {
			WStylePair{ FTextBlockStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTextBlockStyle>(InStyle, InName); }) },
			WStylePair{ FButtonStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FButtonStyle>(InStyle, InName); }) },
			WStylePair{ FComboButtonStyle::StaticStruct()            , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FComboButtonStyle>(InStyle, InName); }) },
			WStylePair{ FComboBoxStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FComboBoxStyle>(InStyle, InName); }) },
			WStylePair{ FHyperlinkStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FHyperlinkStyle>(InStyle, InName); }) },
			WStylePair{ FEditableTextStyle::StaticStruct()           , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FEditableTextStyle>(InStyle, InName); }) },
			WStylePair{ FScrollBarStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBarStyle>(InStyle, InName); }) },
			WStylePair{ FEditableTextBoxStyle::StaticStruct()        , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FEditableTextBoxStyle>(InStyle, InName); }) },
			WStylePair{ FInlineEditableTextBlockStyle::StaticStruct(), WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FInlineEditableTextBlockStyle>(InStyle, InName); }) },
			WStylePair{ FProgressBarStyle::StaticStruct()            , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FProgressBarStyle>(InStyle, InName); }) },
			WStylePair{ FExpandableAreaStyle::StaticStruct()         , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FExpandableAreaStyle>(InStyle, InName); }) },
			WStylePair{ FSearchBoxStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSearchBoxStyle>(InStyle, InName); }) },
			WStylePair{ FSliderStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSliderStyle>(InStyle, InName); }) },
			WStylePair{ FVolumeControlStyle::StaticStruct()          , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FVolumeControlStyle>(InStyle, InName); }) },
			WStylePair{ FInlineTextImageStyle::StaticStruct()        , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FInlineTextImageStyle>(InStyle, InName); }) },
			WStylePair{ FSpinBoxStyle::StaticStruct()                , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSpinBoxStyle>(InStyle, InName); }) },
			WStylePair{ FSplitterStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSplitterStyle>(InStyle, InName); }) },
			WStylePair{ FTableRowStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTableRowStyle>(InStyle, InName); }) },
			WStylePair{ FTableColumnHeaderStyle::StaticStruct()      , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTableColumnHeaderStyle>(InStyle, InName); }) },
			WStylePair{ FHeaderRowStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FHeaderRowStyle>(InStyle, InName); }) },
			WStylePair{ FDockTabStyle::StaticStruct()                , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FDockTabStyle>(InStyle, InName); }) },
			WStylePair{ FScrollBoxStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBoxStyle>(InStyle, InName); }) },
			WStylePair{ FScrollBorderStyle::StaticStruct()           , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBorderStyle>(InStyle, InName); }) },
			WStylePair{ FWindowStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FWindowStyle>(InStyle, InName); }) },
		};


		for (WStylePair widgetStyleUStruct : validWidgetStyleUStructList)
		{
			if (do_ue_py_check_struct(py_type, widgetStyleUStruct.Key))
			{
				ret = widgetStyleUStruct.Value(*self->style_set, FName(name));
				break;
			}
		}
#endif

		if (!ret)
		{
			return PyErr_Format(PyExc_ValueError, "Unsupported FSlateWidgetStyle type. Add it manually.");
		}
	}
	else if (py_ue_is_flinearcolor(py_type))
	{
		ret = py_ue_new_flinearcolor(self->style_set->GetColor(FName(name)));
	}
	else if (PyNumber_Check(py_type))
	{
		ret = PyFloat_FromDouble(self->style_set->GetFloat(FName(name)));
	}
	else
	{
		return PyErr_Format(PyExc_ValueError, "unsupported value type");
	}

	if (!ret)
		return PyErr_Format(PyExc_Exception, "Retrieved style object is in invalid state");

	return ret;

}

static PyMethodDef ue_PyFSlateStyleSet_methods[] = {
	{ "set_content_root", (PyCFunction)py_ue_fslate_style_set_set_content_root, METH_VARARGS, "" },
	{ "set", (PyCFunction)py_ue_fslate_style_set_set, METH_VARARGS, "" },
	{ "get", (PyCFunction)py_ue_fslate_style_set_get, METH_VARARGS, "" },
	{ "register", (PyCFunction)py_ue_fslate_style_set_register, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFSlateStyleSet_str(ue_PyFSlateStyleSet *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SlateStyleSet {'name': %s}>",
		PyUnicode_FromString(TCHAR_TO_UTF8(*self->style_set->GetStyleSetName().ToString())));
}

static PyTypeObject ue_PyFSlateStyleSetType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FSlateStyleSet", /* tp_name */
	sizeof(ue_PyFSlateStyleSet), /* tp_basicsize */
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
	(reprfunc)ue_PyFSlateStyleSet_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FSlateStyleSet",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFSlateStyleSet_methods,             /* tp_methods */
};

static int ue_py_fslate_style_set_init(ue_PyFSlateStyleSet *self, PyObject *args, PyObject *kwargs)
{
	char *name;
	if (!PyArg_ParseTuple(args, "s", &name))
	{
		return -1;
	}
	//TODO: roberto: ikrimae- I think this leaks? How is FSlateIcon destroyed ever?
	self->style_set = new FSlateStyleSet(name);
	return 0;
}

ue_PyFSlateStyleSet* py_ue_new_fslate_style_set(FSlateStyleSet* styleSet)
{
	ue_PyFSlateStyleSet *ret = (ue_PyFSlateStyleSet *)PyObject_New(ue_PyFSlateStyleSet, &ue_PyFSlateStyleSetType);
	ret->style_set = styleSet;
	return ret;
}

void ue_python_init_fslate_style_set(PyObject *ue_module)
{
	ue_PyFSlateStyleSetType.tp_new = PyType_GenericNew;

	ue_PyFSlateStyleSetType.tp_init = (initproc)ue_py_fslate_style_set_init;

	if (PyType_Ready(&ue_PyFSlateStyleSetType) < 0)
		return;

	Py_INCREF(&ue_PyFSlateStyleSetType);
	PyModule_AddObject(ue_module, "FSlateStyleSet", (PyObject *)&ue_PyFSlateStyleSetType);
}