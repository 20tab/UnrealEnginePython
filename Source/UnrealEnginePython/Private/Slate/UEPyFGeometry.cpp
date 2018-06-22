#include "UEPyFGeometry.h"

static PyObject *py_ue_fgeometry_get_local_size(ue_PyFGeometry *self, PyObject * args)
{
	FVector2D size = self->geometry.GetLocalSize();
	return Py_BuildValue("(ff)", size.X, size.Y);
}

static PyObject *py_ue_fgeometry_get_absolute_position(ue_PyFGeometry *self, PyObject * args)
{
#if ENGINE_MINOR_VERSION < 17
	FVector2D size = self->geometry.AbsolutePosition;
#else
	FVector2D size = self->geometry.GetAbsolutePosition();
#endif
	return Py_BuildValue("(ff)", size.X, size.Y);
}

static PyObject *py_ue_fgeometry_absolute_to_local(ue_PyFGeometry *self, PyObject * args)
{
	float x, y;
	if (!PyArg_ParseTuple(args, "(ff)", &x, &y))
		return nullptr;
	FVector2D absolute(x, y);
	FVector2D local = self->geometry.AbsoluteToLocal(absolute);
	return Py_BuildValue((char *)"(ff)", local.X, local.Y);
}

static PyMethodDef ue_PyFGeometry_methods[] = {
	{ "get_local_size", (PyCFunction)py_ue_fgeometry_get_local_size, METH_VARARGS, "" },
	{ "get_absolute_position", (PyCFunction)py_ue_fgeometry_get_absolute_position, METH_VARARGS, "" },
	{ "absolute_to_local", (PyCFunction)py_ue_fgeometry_absolute_to_local, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFGeometry_str(ue_PyFGeometry *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FGeometry '%s'>",
		TCHAR_TO_UTF8(*self->geometry.ToString()));
}

static PyTypeObject ue_PyFGeometryType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FGeometry", /* tp_name */
	sizeof(ue_PyFGeometry), /* tp_basicsize */
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
	(reprfunc)ue_PyFGeometry_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FGeometry",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFGeometry_methods,             /* tp_methods */
};

void ue_python_init_fgeometry(PyObject *ue_module)
{
	ue_PyFGeometryType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyFGeometryType) < 0)
		return;

	Py_INCREF(&ue_PyFGeometryType);
	PyModule_AddObject(ue_module, "FGeometry", (PyObject *)&ue_PyFGeometryType);
}

ue_PyFGeometry *py_ue_is_fgeometry(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFGeometryType))
		return nullptr;
	return (ue_PyFGeometry *)obj;
}

PyObject *py_ue_new_fgeometry(FGeometry geometry)
{
	ue_PyFGeometry *ret = (ue_PyFGeometry *)PyObject_New(ue_PyFGeometry, &ue_PyFGeometryType);
	ret->geometry = geometry;
	return (PyObject *)ret;
}
