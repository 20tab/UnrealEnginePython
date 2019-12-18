#include "UEPyFbxProperty.h"

#if ENGINE_MINOR_VERSION > 12
#if WITH_EDITOR

#include "UEPyFbx.h"

static PyObject *py_ue_fbx_property_get_name(ue_PyFbxProperty *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_property.GetName());
}

static PyObject *py_ue_fbx_property_get_double3(ue_PyFbxProperty *self, PyObject *args)
{
	FbxDouble3 value = self->fbx_property.Get<FbxDouble3>();
	return Py_BuildValue((char *)"(fff)", value[0], value[1], value[2]);
}

static PyObject *py_ue_fbx_property_get_string(ue_PyFbxProperty *self, PyObject *args)
{
	return PyUnicode_FromString(self->fbx_property.Get<FbxString>());
}

static PyObject *py_ue_fbx_property_is_valid(ue_PyFbxProperty *self, PyObject *args)
{
	if (self->fbx_property.IsValid())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyObject *py_ue_fbx_property_get_curve_node(ue_PyFbxProperty *self, PyObject *args)
{
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object))
	{
		return nullptr;
	}

	ue_PyFbxObject *py_fbx_object = py_ue_is_fbx_object(py_object);
	if (!py_fbx_object)
		return PyErr_Format(PyExc_Exception, "argument is not a FbxObject");

	FbxAnimLayer *fbx_anim_layer = FbxCast<FbxAnimLayer>(py_fbx_object->fbx_object);
	if (!fbx_anim_layer)
		return PyErr_Format(PyExc_Exception, "argument is not a FbxAnimLayer");

	FbxAnimCurveNode *fbx_anim_curve_node = self->fbx_property.GetCurveNode(fbx_anim_layer);
	if (!fbx_anim_curve_node)
		Py_RETURN_NONE;
	return py_ue_new_fbx_object(fbx_anim_curve_node);
}

static PyObject *py_ue_fbx_property_get_bool(ue_PyFbxProperty *self, PyObject *args)
{
	if (self->fbx_property.Get<FbxBool>())
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;

}

static PyObject *py_ue_fbx_property_get_int(ue_PyFbxProperty *self, PyObject *args)
{
	return PyLong_FromLong(self->fbx_property.Get<FbxInt>());

}


static PyMethodDef ue_PyFbxProperty_methods[] = {
	{ "get_name", (PyCFunction)py_ue_fbx_property_get_name, METH_VARARGS, "" },
	{ "get_double3", (PyCFunction)py_ue_fbx_property_get_double3, METH_VARARGS, "" },
	{ "get_string", (PyCFunction)py_ue_fbx_property_get_string, METH_VARARGS, "" },
	{ "get_bool", (PyCFunction)py_ue_fbx_property_get_bool, METH_VARARGS, "" },
	{ "get_int", (PyCFunction)py_ue_fbx_property_get_int, METH_VARARGS, "" },
	{ "is_valid", (PyCFunction)py_ue_fbx_property_is_valid, METH_VARARGS, "" },
	{ "get_curve_node", (PyCFunction)py_ue_fbx_property_get_curve_node, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxPropertyType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxProperty", /* tp_name */
	sizeof(ue_PyFbxProperty),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	0,   /* tp_dealloc */
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
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FbxProperty", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxProperty_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_property_init(ue_PyFbxProperty *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "instantiating a new FbxProperty is currently not supported");
	return -1;
}

void ue_python_init_fbx_property(PyObject *ue_module)
{
	ue_PyFbxPropertyType.tp_new = PyType_GenericNew;;
	ue_PyFbxPropertyType.tp_init = (initproc)py_ue_fbx_property_init;
	if (PyType_Ready(&ue_PyFbxPropertyType) < 0)
		return;

	Py_INCREF(&ue_PyFbxPropertyType);
	PyModule_AddObject(ue_module, "FbxProperty", (PyObject *)&ue_PyFbxPropertyType);
}

PyObject *py_ue_new_fbx_property(FbxProperty fbx_property)
{
	ue_PyFbxProperty *ret = (ue_PyFbxProperty *)PyObject_New(ue_PyFbxProperty, &ue_PyFbxPropertyType);
	ret->fbx_property = fbx_property;
	return (PyObject *)ret;
}

ue_PyFbxProperty *py_ue_is_fbx_property(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFbxPropertyType))
		return nullptr;
	return (ue_PyFbxProperty *)obj;
}
#endif
#endif
