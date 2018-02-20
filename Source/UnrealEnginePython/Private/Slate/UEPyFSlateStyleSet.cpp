#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyFSlateStyleSet.h"
#include "SlateTypes.h"
#include "Map.h"

static PyObject *py_ue_fslate_style_set_set_content_root(ue_PyFSlateStyleSet *self, PyObject * args)
{
	char *path;
	if (!PyArg_ParseTuple(args, "s:set_content_root", &path))
		return NULL;

	self->style_set->SetContentRoot(path);

	Py_INCREF(Py_None);
	return Py_None;
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

namespace {
    template <typename WidgetStyleType> 
    void pySetWidgetStyle(FSlateStyleSet& InStyle, const FName PropertyName, void* InStyleDefintion)
    {
        InStyle.Set<WidgetStyleType>(PropertyName, *static_cast<WidgetStyleType*>(InStyleDefintion));
    }
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

    if (ue_py_check_childstruct<FSlateWidgetStyle>(py_value))
    {
        typedef TFunction<void (FSlateStyleSet&, FName, void*)> WStyleSetter;
        typedef TPair<UScriptStruct*, WStyleSetter> WStylePair;
#if ENGINE_MINOR_VERSION > 15
        static const WStylePair validWidgetStyleUStructList[] = {
            WStylePair{ FTextBlockStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FTextBlockStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FButtonStyle::StaticStruct()                 , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FButtonStyle>                 (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FComboButtonStyle::StaticStruct()            , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FComboButtonStyle>            (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FComboBoxStyle::StaticStruct()               , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FComboBoxStyle>               (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FHyperlinkStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FHyperlinkStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FEditableTextStyle::StaticStruct()           , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FEditableTextStyle>           (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FScrollBarStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FScrollBarStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FEditableTextBoxStyle::StaticStruct()        , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FEditableTextBoxStyle>        (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FInlineEditableTextBlockStyle::StaticStruct(), WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FInlineEditableTextBlockStyle>(InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FProgressBarStyle::StaticStruct()            , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FProgressBarStyle>            (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FExpandableAreaStyle::StaticStruct()         , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FExpandableAreaStyle>         (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FSearchBoxStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FSearchBoxStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FSliderStyle::StaticStruct()                 , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FSliderStyle>                 (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FVolumeControlStyle::StaticStruct()          , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FVolumeControlStyle>          (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FInlineTextImageStyle::StaticStruct()        , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FInlineTextImageStyle>        (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FSpinBoxStyle::StaticStruct()                , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FSpinBoxStyle>                (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FSplitterStyle::StaticStruct()               , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FSplitterStyle>               (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FTableRowStyle::StaticStruct()               , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FTableRowStyle>               (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FTableColumnHeaderStyle::StaticStruct()      , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FTableColumnHeaderStyle>      (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FHeaderRowStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FHeaderRowStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FDockTabStyle::StaticStruct()                , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FDockTabStyle>                (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FScrollBoxStyle::StaticStruct()              , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FScrollBoxStyle>              (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FScrollBorderStyle::StaticStruct()           , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FScrollBorderStyle>           (InStyle, InName,InWidgetStyleDef); }) },
            WStylePair{ FWindowStyle::StaticStruct()                 , WStyleSetter([](FSlateStyleSet& InStyle, FName InName, void* InWidgetStyleDef) { pySetWidgetStyle<FWindowStyle>                 (InStyle, InName,InWidgetStyleDef); }) },
        };

        
        for(WStylePair widgetStyleUStruct : validWidgetStyleUStructList)
        {
            if (void* struct_data = do_ue_py_check_struct(py_value, widgetStyleUStruct.Key))
            {
                widgetStyleUStruct.Value(*self->style_set, FName(name), struct_data);
                Py_RETURN_NONE;
            }
        }
#endif
        return PyErr_Format(PyExc_ValueError, "Unsupported FSlateWidgetStyle type. Add it manually to python plugin.");
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

namespace {
    template <typename WidgetStyleType> 
    PyObject* pyGetWidgetStyle(FSlateStyleSet& InStyle, FName PropertyName)
    {
        const WidgetStyleType styleWidgetStyle = InStyle.GetWidgetStyle<WidgetStyleType>(PropertyName);
        return py_ue_new_uscriptstruct(WidgetStyleType::StaticStruct(), (uint8*)&styleWidgetStyle);
    }
}

static PyObject *py_ue_fslate_style_set_get(ue_PyFSlateStyleSet *self, PyObject * args)
{
    if (!(self && self->style_set))
        return PyErr_Format(PyExc_Exception, "fstyleset is in invalid state");

	char *name         = nullptr;
    PyObject *py_type  = nullptr;

	if (!PyArg_ParseTuple(args, "sO:get", &name, &py_type))
		return NULL;

    PyObject *ret = nullptr;

	if (ue_py_check_struct<FSlateSound>(py_type))
	{
        const FSlateSound& styleSound = self->style_set->GetSound(FName(name));
        ret = py_ue_new_uscriptstruct(FSlateSound::StaticStruct(), (uint8*)&styleSound);
	}
	else if (ue_py_check_struct<FSlateBrush>(py_type))
	{
        if (const FSlateBrush* styleBrush = self->style_set->GetBrush(FName(name)))
        {
            ret = py_ue_new_uscriptstruct(FSlateBrush::StaticStruct(), (uint8*)styleBrush);
        }
	}
	else if (ue_py_check_struct<FSlateColor>(py_type))
	{
        const FSlateColor styleSlateColor = self->style_set->GetSlateColor(FName(name));
        ret = py_ue_new_uscriptstruct(FSlateColor::StaticStruct(), (uint8*)&styleSlateColor);
	}
    else if (ue_py_check_struct<FSlateFontInfo>(py_type))
	{
        const FSlateFontInfo styleFontInfo = self->style_set->GetFontStyle(FName(name));
        ret = py_ue_new_uscriptstruct(FSlateFontInfo::StaticStruct(), (uint8*)&styleFontInfo);
	}
    else if (ue_py_check_childstruct<FSlateWidgetStyle>(py_type))
    {

        typedef TFunction<PyObject* (FSlateStyleSet&, FName)> WStyleGetter;
        typedef TPair<UScriptStruct*, WStyleGetter> WStylePair;
#if ENGINE_MINOR_VERSION > 15
        static const WStylePair validWidgetStyleUStructList[] = {
            WStylePair{ FTextBlockStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTextBlockStyle>              (InStyle, InName); }) },
            WStylePair{ FButtonStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FButtonStyle>                 (InStyle, InName); }) },
            WStylePair{ FComboButtonStyle::StaticStruct()            , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FComboButtonStyle>            (InStyle, InName); }) },
            WStylePair{ FComboBoxStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FComboBoxStyle>               (InStyle, InName); }) },
            WStylePair{ FHyperlinkStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FHyperlinkStyle>              (InStyle, InName); }) },
            WStylePair{ FEditableTextStyle::StaticStruct()           , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FEditableTextStyle>           (InStyle, InName); }) },
            WStylePair{ FScrollBarStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBarStyle>              (InStyle, InName); }) },
            WStylePair{ FEditableTextBoxStyle::StaticStruct()        , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FEditableTextBoxStyle>        (InStyle, InName); }) },
            WStylePair{ FInlineEditableTextBlockStyle::StaticStruct(), WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FInlineEditableTextBlockStyle>(InStyle, InName); }) },
            WStylePair{ FProgressBarStyle::StaticStruct()            , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FProgressBarStyle>            (InStyle, InName); }) },
            WStylePair{ FExpandableAreaStyle::StaticStruct()         , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FExpandableAreaStyle>         (InStyle, InName); }) },
            WStylePair{ FSearchBoxStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSearchBoxStyle>              (InStyle, InName); }) },
            WStylePair{ FSliderStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSliderStyle>                 (InStyle, InName); }) },
            WStylePair{ FVolumeControlStyle::StaticStruct()          , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FVolumeControlStyle>          (InStyle, InName); }) },
            WStylePair{ FInlineTextImageStyle::StaticStruct()        , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FInlineTextImageStyle>        (InStyle, InName); }) },
            WStylePair{ FSpinBoxStyle::StaticStruct()                , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSpinBoxStyle>                (InStyle, InName); }) },
            WStylePair{ FSplitterStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FSplitterStyle>               (InStyle, InName); }) },
            WStylePair{ FTableRowStyle::StaticStruct()               , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTableRowStyle>               (InStyle, InName); }) },
            WStylePair{ FTableColumnHeaderStyle::StaticStruct()      , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FTableColumnHeaderStyle>      (InStyle, InName); }) },
            WStylePair{ FHeaderRowStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FHeaderRowStyle>              (InStyle, InName); }) },
            WStylePair{ FDockTabStyle::StaticStruct()                , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FDockTabStyle>                (InStyle, InName); }) },
            WStylePair{ FScrollBoxStyle::StaticStruct()              , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBoxStyle>              (InStyle, InName); }) },
            WStylePair{ FScrollBorderStyle::StaticStruct()           , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FScrollBorderStyle>           (InStyle, InName); }) },
            WStylePair{ FWindowStyle::StaticStruct()                 , WStyleGetter([](FSlateStyleSet& InStyle, FName InName) { return pyGetWidgetStyle<FWindowStyle>                 (InStyle, InName); }) },
        };

        
        for(WStylePair widgetStyleUStruct : validWidgetStyleUStructList)
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
    ret->style_set           = styleSet;
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