
#include "UEPyFStringAssetReference.h"

static PyObject *py_ue_fstring_asset_reference_get_asset_name(ue_PyFStringAssetReference *self, PyObject * args)
{
#if ENGINE_MINOR_VERSION > 13
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->fstring_asset_reference.GetAssetName()));
#else
	return PyUnicode_FromString(TCHAR_TO_UTF8(*FPackageName::ObjectPathToObjectName(self->fstring_asset_reference.ToString())));
#endif
}

static PyObject *py_ue_fstring_asset_reference_get_long_package_name(ue_PyFStringAssetReference *self, PyObject * args)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*self->fstring_asset_reference.GetLongPackageName()));
}

static PyMethodDef ue_PyFStringAssetReference_methods[] = {
	{ "get_asset_name", (PyCFunction)py_ue_fstring_asset_reference_get_asset_name, METH_VARARGS, "" },
	{ "get_long_package_name", (PyCFunction)py_ue_fstring_asset_reference_get_long_package_name, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyFStringAssetReference_str(ue_PyFStringAssetReference *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FStringAssetReference {'asset_name': %s}>",
#if ENGINE_MINOR_VERSION > 13
		TCHAR_TO_UTF8(*self->fstring_asset_reference.GetAssetName()));
#else
		TCHAR_TO_UTF8(*FPackageName::ObjectPathToObjectName(self->fstring_asset_reference.ToString())));
#endif
}

static PyTypeObject ue_PyFStringAssetReferenceType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FStringAssetReference", /* tp_name */
	sizeof(ue_PyFVector), /* tp_basicsize */
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
	(reprfunc)ue_PyFStringAssetReference_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine FStringAssetReference",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFStringAssetReference_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_fstring_asset_reference_init(ue_PyFStringAssetReference *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object))
		return -1;

	if (PyUnicodeOrString_Check(py_object))
	{
		const char *value = UEPyUnicode_AsUTF8(py_object);
		self->fstring_asset_reference = FStringAssetReference(FString(UTF8_TO_TCHAR(value)));
		return 0;
	}

	UObject *u_object = ue_py_check_type<UObject>(py_object);
	if (!u_object)
	{
		PyErr_SetString(PyExc_Exception, "argument is not a UObject or a string");
		return -1;
	}

	self->fstring_asset_reference = FStringAssetReference(u_object);
	return 0;
}



void ue_python_init_fstring_asset_reference(PyObject *ue_module)
{
	ue_PyFStringAssetReferenceType.tp_new = PyType_GenericNew;

	ue_PyFStringAssetReferenceType.tp_init = (initproc)ue_py_fstring_asset_reference_init;


	if (PyType_Ready(&ue_PyFStringAssetReferenceType) < 0)
		return;

	Py_INCREF(&ue_PyFStringAssetReferenceType);
	PyModule_AddObject(ue_module, "FStringAssetReference", (PyObject *)&ue_PyFStringAssetReferenceType);
	PyModule_AddObject(ue_module, "FSoftObjectPath", (PyObject *)&ue_PyFStringAssetReferenceType);
}

PyObject *py_ue_new_fstring_asset_reference(FStringAssetReference ref)
{
	ue_PyFStringAssetReference *ret = (ue_PyFStringAssetReference *)PyObject_New(ue_PyFStringAssetReference, &ue_PyFStringAssetReferenceType);
	ret->fstring_asset_reference = ref;
	return (PyObject *)ret;
}

ue_PyFStringAssetReference *py_ue_is_fstring_asset_reference(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFStringAssetReferenceType))
		return nullptr;
	return (ue_PyFStringAssetReference *)obj;
}
