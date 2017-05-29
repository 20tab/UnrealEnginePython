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

static PyMethodDef ue_PyFSlateStyleSet_methods[] = {
	{ "set_content_root", (PyCFunction)py_ue_fslate_style_set_set_content_root, METH_VARARGS, "" },
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