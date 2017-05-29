#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyFSlateStyleSet.h"

static PyObject *py_ue_fslate_style_set_set_content_root(ue_PyFSlateStyleSet *self, PyObject * args) {
	char *path;
	if (!PyArg_ParseTuple(args, "s:set_content_root", &path))
		return NULL;

	self->style_set->SetContentRoot(path);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_fslate_style_set_register(ue_PyFSlateStyleSet *self, PyObject * args) {

	FSlateStyleRegistry::RegisterSlateStyle(*self->style_set);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_fslate_style_set_set(ue_PyFSlateStyleSet *self, PyObject * args) {
	char *name;
	PyObject *py_value;
	if (!PyArg_ParseTuple(args, "sO:set", &name, &py_value))
		return NULL;

	FSlateSound *slate_sound = ue_py_check_struct<FSlateSound>(py_value);
	if (slate_sound) {
		self->style_set->Set(FName(name), *slate_sound);
		Py_RETURN_NONE;
	}

	FSlateBrush *slate_brush = ue_py_check_struct<FSlateBrush>(py_value);
	if (slate_brush) {
		self->style_set->Set(FName(name), slate_brush);
		Py_RETURN_NONE;
	}

	FSlateColor *slate_color = ue_py_check_struct<FSlateColor>(py_value);
	if (slate_brush) {
		self->style_set->Set(FName(name), *slate_color);
		Py_RETURN_NONE;
	}

	FSlateFontInfo *slate_font = ue_py_check_struct<FSlateFontInfo>(py_value);
	if (slate_font) {
		self->style_set->Set(FName(name), *slate_font);
		Py_RETURN_NONE;
	}

	ue_PyFLinearColor *py_linear_color = py_ue_is_flinearcolor(py_value);
	if (py_linear_color) {
		self->style_set->Set(FName(name), py_linear_color->color);
		Py_RETURN_NONE;
	}

	ue_PyFColor *py_color = py_ue_is_fcolor(py_value);
	if (py_color) {
		self->style_set->Set(FName(name), py_color->color);
		Py_RETURN_NONE;
	}

	if (PyNumber_Check(py_value)) {
		PyObject *py_float = PyNumber_Float(py_value);
		self->style_set->Set(FName(name), (float)PyFloat_AsDouble(py_float));
		Py_DECREF(py_float);
		Py_RETURN_NONE;
	}

	return PyErr_Format(PyExc_ValueError, "unsupported value type");
}

static PyMethodDef ue_PyFSlateStyleSet_methods[] = {
	{ "set_content_root", (PyCFunction)py_ue_fslate_style_set_set_content_root, METH_VARARGS, "" },
	{ "set", (PyCFunction)py_ue_fslate_style_set_set, METH_VARARGS, "" },
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

static int ue_py_fslate_style_set_init(ue_PyFSlateStyleSet *self, PyObject *args, PyObject *kwargs) {
	char *name;
	if (!PyArg_ParseTuple(args, "s", &name)) {
		return -1;
	}

	self->style_set = new FSlateStyleSet(name);
	return 0;
}

void ue_python_init_fslate_style_set(PyObject *ue_module) {
	ue_PyFSlateStyleSetType.tp_new = PyType_GenericNew;

	ue_PyFSlateStyleSetType.tp_init = (initproc)ue_py_fslate_style_set_init;

	if (PyType_Ready(&ue_PyFSlateStyleSetType) < 0)
		return;

	Py_INCREF(&ue_PyFSlateStyleSetType);
	PyModule_AddObject(ue_module, "FSlateStyleSet", (PyObject *)&ue_PyFSlateStyleSetType);
}