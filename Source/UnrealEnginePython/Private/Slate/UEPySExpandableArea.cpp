
#include "UEPySExpandableArea.h"
#include "UEPySCheckBox.h"


static PyObject *py_ue_sexpandable_area_is_expanded(ue_PySCheckBox *self, PyObject * args) 
{
    ue_py_slate_cast(SExpandableArea);
	if (py_SExpandableArea->IsExpanded()) {
		Py_RETURN_TRUE;
	}
	
	Py_RETURN_FALSE;
}

static PyObject *py_ue_sexpandable_area_set_expanded(ue_PySCheckBox *self, PyObject * args) 
{
    ue_py_slate_cast(SExpandableArea);
	PyObject *py_bool;
	if (!PyArg_ParseTuple(args, "O:set_expanded", &py_bool))
	{
		return NULL;
	}

    py_SExpandableArea->SetExpanded(PyObject_IsTrue(py_bool) ? true : false);

	Py_RETURN_NONE;
}

static PyMethodDef ue_PySExpandableArea_methods[] = {
    { "is_expanded",  (PyCFunction)py_ue_sexpandable_area_is_expanded, METH_VARARGS, "" },
    { "set_expanded", (PyCFunction)py_ue_sexpandable_area_set_expanded, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySExpandableAreaType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SExpandableArea", /* tp_name */
	sizeof(ue_PySExpandableArea), /* tp_basicsize */
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
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SExpandableArea",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySExpandableArea_methods,             /* tp_methods */
};

static int ue_py_sexpandable_area_init(ue_PySExpandableArea *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SExpandableArea);
    ue_py_slate_farguments_argument_struct_ptr("style", Style, FExpandableAreaStyle);
    ue_py_slate_farguments_attribute_struct   ("border_background_color", BorderBackgroundColor, FSlateColor);
    ue_py_slate_farguments_argument_struct_ptr("border_image", BorderImage, FSlateBrush);
    ue_py_slate_farguments_attribute_struct   ("body_border_background_color", BodyBorderBackgroundColor, FSlateColor);
    ue_py_slate_farguments_argument_struct_ptr("body_border_image", BodyBorderImage, FSlateBrush);
    ue_py_slate_farguments_named_slot         ("header_content", HeaderContent);
    ue_py_slate_farguments_named_slot         ("body_content", BodyContent);
    ue_py_slate_farguments_attribute_text     ("area_title", AreaTitle);
    ue_py_slate_farguments_argument_bool      ("initially_collapsed", InitiallyCollapsed);
    ue_py_slate_farguments_argument_float     ("min_width", MinWidth);
    ue_py_slate_farguments_argument_float     ("max_height", MaxHeight);
    ue_py_slate_farguments_attribute_struct   ("area_title_padding", AreaTitlePadding, FMargin);
    ue_py_slate_farguments_attribute_struct   ("header_padding", HeaderPadding, FMargin);
    ue_py_slate_farguments_attribute_struct   ("padding", Padding, FMargin);
    ue_py_slate_farguments_event              ("on_area_expansion_changed", OnAreaExpansionChanged, FOnBooleanValueChanged, OnBoolChanged);
    ue_py_slate_farguments_struct             ("area_title_font", AreaTitleFont, FSlateFontInfo);

	ue_py_snew(SExpandableArea);
	return 0;
}

void ue_python_init_sexpandable_area(PyObject *ue_module)
{
    ue_PySExpandableAreaType.tp_init = (initproc)ue_py_sexpandable_area_init;
    ue_PySExpandableAreaType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySExpandableAreaType) < 0)
		return;

	Py_INCREF(&ue_PySExpandableAreaType);
	PyModule_AddObject(ue_module, "SExpandableArea", (PyObject *)&ue_PySExpandableAreaType);
}
