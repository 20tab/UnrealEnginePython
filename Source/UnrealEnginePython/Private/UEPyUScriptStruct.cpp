

#include "UnrealEnginePythonPrivatePCH.h"


static PyObject *py_ue_uscriptstruct_get_field(ue_PyUScriptStruct *self, PyObject * args) {
	char *name;
	if (!PyArg_ParseTuple(args, "s:get_field", &name)) {
		return NULL;
	}

	UProperty *u_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);

	return ue_py_convert_property(u_property, self->data);
}

static PyObject *py_ue_uscriptstruct_set_field(ue_PyUScriptStruct *self, PyObject * args) {
	char *name;
	PyObject *value;
	if (!PyArg_ParseTuple(args, "sO:set_field", &name, &value)) {
		return NULL;
	}

	UProperty *u_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);


	if (!ue_py_convert_pyobject(value, u_property, self->data)) {
		return PyErr_Format(PyExc_Exception, "unable to set property %s", name);
	}

	Py_INCREF(Py_None);
	return Py_None;

}

static PyObject *py_ue_uscriptstruct_fields(ue_PyUScriptStruct *self, PyObject * args) {
	PyObject *ret = PyList_New(0);

	for (TFieldIterator<UProperty> PropIt(self->u_struct); PropIt; ++PropIt)
	{
		UProperty* property = *PropIt;
		PyObject *property_name = PyUnicode_FromString(TCHAR_TO_UTF8(*property->GetName()));
		PyList_Append(ret, property_name);
		Py_DECREF(property_name);
	}

	return ret;
}

static PyObject *py_ue_uscriptstruct_get_struct(ue_PyUScriptStruct *self, PyObject * args) {
	ue_PyUObject *ret = ue_get_python_wrapper(self->u_struct);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "PyUObject is in invalid state");
	Py_INCREF(ret);
	return (PyObject *)ret;
}

static PyMethodDef ue_PyUScriptStruct_methods[] = {
	{ "get_field", (PyCFunction)py_ue_uscriptstruct_get_field, METH_VARARGS, "" },
	{ "set_field", (PyCFunction)py_ue_uscriptstruct_set_field, METH_VARARGS, "" },
	{ "fields", (PyCFunction)py_ue_uscriptstruct_fields, METH_VARARGS, "" },
	{ "get_struct", (PyCFunction)py_ue_uscriptstruct_get_struct, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyUScriptStruct_str(ue_PyUScriptStruct *self)
{
	return PyUnicode_FromFormat("<unreal_engine.UScriptStruct {'struct': '%s', 'size': %d}>",
		TCHAR_TO_UTF8(*self->u_struct->GetName()), self->u_struct->GetStructureSize());
}

static UProperty *get_field_from_name(UScriptStruct *u_struct, char *name) {
	FString attr = UTF8_TO_TCHAR(name);
	UProperty *u_property = u_struct->FindPropertyByName(FName(*attr));
	if (u_property)
		return u_property;
	// if the property is not found, attempt to search for name_XXXX
	attr += FString("_");
	for (TFieldIterator<UProperty> prop(u_struct); prop; ++prop)
	{
		UProperty *property = *prop;
		if (property->GetName().StartsWith(attr)) {
			return property;
		}
	}

	return nullptr;
}

static PyObject *ue_PyUScriptStruct_getattro(ue_PyUScriptStruct *self, PyObject *attr_name) {
	PyObject *ret = PyObject_GenericGetAttr((PyObject *)self, attr_name);
	if (!ret) {
		if (PyUnicodeOrString_Check(attr_name)) {
			char *attr = PyUnicode_AsUTF8(attr_name);
			// first check for property
			UProperty *u_property = get_field_from_name(self->u_struct, attr);
			if (u_property) {
				// swallow previous exception
				PyErr_Clear();
				return ue_py_convert_property(u_property, self->data);
			}
		}
	}
	return ret;
}

static int ue_PyUScriptStruct_setattro(ue_PyUScriptStruct *self, PyObject *attr_name, PyObject *value) {
	// first of all check for UProperty
	if (PyUnicodeOrString_Check(attr_name)) {
		char *attr = PyUnicode_AsUTF8(attr_name);
		// first check for property
		UProperty *u_property = get_field_from_name(self->u_struct, attr);
		if (u_property) {
			if (ue_py_convert_pyobject(value, u_property, self->data)) {
				return 0;
			}
			PyErr_SetString(PyExc_ValueError, "invalid value for UProperty");
			return -1;
		}
	}
	return PyObject_GenericSetAttr((PyObject *)self, attr_name, value);
}

// destructor
static void ue_PyUScriptStruct_dealloc(ue_PyUScriptStruct *self) {
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyUScriptStruct %p with size %d"), self, self->u_struct->GetStructureSize());
#endif
	FMemory::Free(self->data);
	Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyTypeObject ue_PyUScriptStructType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UScriptStruct", /* tp_name */
	sizeof(ue_PyUScriptStruct), /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_PyUScriptStruct_dealloc,       /* tp_dealloc */
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
	(reprfunc)ue_PyUScriptStruct_str,                         /* tp_str */
	(getattrofunc)ue_PyUScriptStruct_getattro, /* tp_getattro */
	(setattrofunc)ue_PyUScriptStruct_setattro, /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine Editor UScriptStruct",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyUScriptStruct_methods,             /* tp_methods */
	0,
	0,
};

static int ue_py_uscriptstruct_init(ue_PyUScriptStruct *self, PyObject *args, PyObject *kwargs) {
	PyObject *py_struct;
	if (!PyArg_ParseTuple(args, "O", &py_struct))
		return -1;

	if (!ue_is_pyuobject(py_struct)) {
		PyErr_SetString(PyExc_Exception, "argument is not a UScriptStruct");
		return -1;
	}

	ue_PyUObject *py_u_obj = (ue_PyUObject *)py_struct;
	if (!py_u_obj->ue_object->IsA<UScriptStruct>()) {
		PyErr_SetString(PyExc_Exception, "argument is not a UScriptStruct");
		return -1;
	}

	self->u_struct = (UScriptStruct *)py_u_obj->ue_object;
	self->data = (uint8*)FMemory::Malloc(self->u_struct->GetStructureSize());
	self->u_struct->InitializeStruct(self->data);
#if WITH_EDITOR
	self->u_struct->InitializeDefaultValue(self->data);
#endif
	return 0;
}


void ue_python_init_uscriptstruct(PyObject *ue_module) {
	ue_PyUScriptStructType.tp_new = PyType_GenericNew;

	ue_PyUScriptStructType.tp_init = (initproc)ue_py_uscriptstruct_init;

	if (PyType_Ready(&ue_PyUScriptStructType) < 0)
		return;

	Py_INCREF(&ue_PyUScriptStructType);
	PyModule_AddObject(ue_module, "UScriptStruct", (PyObject *)&ue_PyUScriptStructType);
}

PyObject *py_ue_new_uscriptstruct(UScriptStruct *u_struct, uint8 *data) {
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = u_struct;
	uint8 *struct_data = (uint8*)FMemory::Malloc(u_struct->GetStructureSize());
	FMemory::Memcpy(struct_data, data, u_struct->GetStructureSize());
	ret->data = struct_data;
	return (PyObject *)ret;
}

ue_PyUScriptStruct *py_ue_is_uscriptstruct(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUScriptStructType))
		return nullptr;
	return (ue_PyUScriptStruct *)obj;
}