
#include "UEPySColorBlock.h"

static PyMethodDef ue_PySColorBlock_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySColorBlockType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SColorBlock", /* tp_name */
	sizeof(ue_PySColorBlock), /* tp_basicsize */
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
	"Unreal Engine SColorBlock",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySColorBlock_methods,             /* tp_methods */
};

static int ue_py_scolor_block_init(ue_PySColorBlock *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SColorBlock);

	ue_py_slate_farguments_flinear_color("color", Color);
	ue_py_slate_farguments_bool("color_is_hsv", ColorIsHSV);
	ue_py_slate_farguments_bool("ignore_alpha", IgnoreAlpha);
	ue_py_slate_farguments_event("on_mouse_button_down", OnMouseButtonDown, FPointerEventHandler, OnMouseEvent);
	ue_py_slate_farguments_bool("show_background_for_alpha", ShowBackgroundForAlpha);
	ue_py_slate_farguments_fvector2d("size", Size);
	ue_py_slate_farguments_bool("use_srgb", UseSRGB);

	ue_py_snew(SColorBlock);
	return 0;
}

void ue_python_init_scolor_block(PyObject *ue_module)
{

	ue_PySColorBlockType.tp_init = (initproc)ue_py_scolor_block_init;

	ue_PySColorBlockType.tp_base = &ue_PySLeafWidgetType;

	if (PyType_Ready(&ue_PySColorBlockType) < 0)
		return;

	Py_INCREF(&ue_PySColorBlockType);
	PyModule_AddObject(ue_module, "SColorBlock", (PyObject *)&ue_PySColorBlockType);
}
