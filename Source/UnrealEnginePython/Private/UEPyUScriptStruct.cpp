// Copyright 20Tab S.r.l.

#include "UEPyUScriptStruct.h"


static PyObject *py_ue_uscriptstruct_get_field(ue_PyUScriptStruct *self, PyObject * args)
{
	char *name;
	int index = 0;
	if (!PyArg_ParseTuple(args, "s|i:get_field", &name, &index))
	{
		return nullptr;
	}

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	FProperty *f_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!f_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);

	return ue_py_convert_property(f_property, self->u_struct_ptr, index);
#else
	UProperty *u_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);

	return ue_py_convert_property(u_property, self->u_struct_ptr, index);
#endif
}

static PyObject *py_ue_uscriptstruct_get_field_array_dim(ue_PyUScriptStruct *self, PyObject * args)
{
	char *name;
	if (!PyArg_ParseTuple(args, "s:get_field_array_dim", &name))
	{
		return nullptr;
	}

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	FProperty *f_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!f_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);

	return PyLong_FromLongLong(f_property->ArrayDim);
#else
	UProperty *u_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);

	return PyLong_FromLongLong(u_property->ArrayDim);
#endif
}

static PyObject *py_ue_uscriptstruct_set_field(ue_PyUScriptStruct *self, PyObject * args)
{
	char *name;
	PyObject *value;
	int index = 0;
	if (!PyArg_ParseTuple(args, "sO|i:set_field", &name, &value, &index))
	{
		return nullptr;
	}

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	FProperty *f_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!f_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);


	if (!ue_py_convert_pyobject(value, f_property, self->u_struct_ptr, index))
	{
		return PyErr_Format(PyExc_Exception, "unable to set property %s", name);
	}
#else
	UProperty *u_property = self->u_struct->FindPropertyByName(FName(UTF8_TO_TCHAR(name)));
	if (!u_property)
		return PyErr_Format(PyExc_Exception, "unable to find property %s", name);


	if (!ue_py_convert_pyobject(value, u_property, self->u_struct_ptr, index))
	{
		return PyErr_Format(PyExc_Exception, "unable to set property %s", name);
	}
#endif

	Py_RETURN_NONE;

}

static PyObject *py_ue_uscriptstruct_fields(ue_PyUScriptStruct *self, PyObject * args)
{
	PyObject *ret = PyList_New(0);

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	for (TFieldIterator<FProperty> PropIt(self->u_struct); PropIt; ++PropIt)
	{
		FProperty* property = *PropIt;
		PyObject *property_name = PyUnicode_FromString(TCHAR_TO_UTF8(*property->GetName()));
		PyList_Append(ret, property_name);
		Py_DECREF(property_name);
	}
#else
	for (TFieldIterator<UProperty> PropIt(self->u_struct); PropIt; ++PropIt)
	{
		UProperty* property = *PropIt;
		PyObject *property_name = PyUnicode_FromString(TCHAR_TO_UTF8(*property->GetName()));
		PyList_Append(ret, property_name);
		Py_DECREF(property_name);
	}
#endif

	return ret;
}

static PyObject *py_ue_uscriptstruct_get_struct(ue_PyUScriptStruct *self, PyObject * args)
{
	Py_RETURN_UOBJECT(self->u_struct);
}

static PyObject *py_ue_uscriptstruct_clone(ue_PyUScriptStruct *, PyObject *);

PyObject *py_ue_uscriptstruct_as_dict(ue_PyUScriptStruct * self, PyObject * args)
{

	PyObject *py_bool = nullptr;
	if (!PyArg_ParseTuple(args, "|O:as_dict", &py_bool))
	{
		return nullptr;
	}

	static const FName DisplayNameKey(TEXT("DisplayName"));

	PyObject *py_struct_dict = PyDict_New();
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	TFieldIterator<FProperty> SArgs(self->u_struct);
#else
	TFieldIterator<UProperty> SArgs(self->u_struct);
#endif
	for (; SArgs; ++SArgs)
	{
		PyObject *struct_value = ue_py_convert_property(*SArgs, self->u_struct_ptr, 0);
		if (!struct_value)
		{
			Py_DECREF(py_struct_dict);
			return NULL;
		}
		FString prop_name = SArgs->GetName();
#if WITH_EDITOR
		if (py_bool && PyObject_IsTrue(py_bool))
		{

			if (SArgs->HasMetaData(DisplayNameKey))
			{
				FString display_name = SArgs->GetMetaData(DisplayNameKey);
				if (display_name.Len() > 0)
					prop_name = display_name;
			}
		}
#endif
		PyDict_SetItemString(py_struct_dict, TCHAR_TO_UTF8(*prop_name), struct_value);
	}
	return py_struct_dict;
}

static PyObject *py_ue_uscriptstruct_ref(ue_PyUScriptStruct *, PyObject *);




static PyMethodDef ue_PyUScriptStruct_methods[] = {
	{ "get_field", (PyCFunction)py_ue_uscriptstruct_get_field, METH_VARARGS, "" },
	{ "set_field", (PyCFunction)py_ue_uscriptstruct_set_field, METH_VARARGS, "" },
	{ "get_field_array_dim", (PyCFunction)py_ue_uscriptstruct_get_field_array_dim, METH_VARARGS, "" },
	{ "fields", (PyCFunction)py_ue_uscriptstruct_fields, METH_VARARGS, "" },
	{ "get_struct", (PyCFunction)py_ue_uscriptstruct_get_struct, METH_VARARGS, "" },
	{ "clone", (PyCFunction)py_ue_uscriptstruct_clone, METH_VARARGS, "" },
	{ "as_dict", (PyCFunction)py_ue_uscriptstruct_as_dict, METH_VARARGS, "" },
	{ "ref", (PyCFunction)py_ue_uscriptstruct_ref, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyUScriptStruct_str(ue_PyUScriptStruct *self)
{
	return PyUnicode_FromFormat("<unreal_engine.UScriptStruct {'struct': '%s', 'size': %d, 'ptr': %p}>",
		TCHAR_TO_UTF8(*self->u_struct->GetName()), self->u_struct->GetStructureSize(), self->u_struct_ptr);
}

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
static FProperty *get_field_from_name(UScriptStruct *u_struct, char *name)
#else
static UProperty *get_field_from_name(UScriptStruct *u_struct, char *name)
#endif
{
	FString attr = UTF8_TO_TCHAR(name);
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	FProperty *f_property = u_struct->FindPropertyByName(FName(*attr));
	if (f_property)
		return f_property;
#else
	UProperty *u_property = u_struct->FindPropertyByName(FName(*attr));
	if (u_property)
		return u_property;
#endif

#if WITH_EDITOR
	static const FName DisplayNameKey(TEXT("DisplayName"));

	// if the property is not found, attempt to search for DisplayName
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
	for (TFieldIterator<FProperty> prop(u_struct); prop; ++prop)
	{
		FProperty *property = *prop;
#else
	for (TFieldIterator<UProperty> prop(u_struct); prop; ++prop)
	{
		UProperty *property = *prop;
#endif
		if (property->HasMetaData(DisplayNameKey))
		{
			FString display_name = property->GetMetaData(DisplayNameKey);
			if (display_name.Len() > 0 && attr.Equals(display_name))
			{
				return property;
			}
		}
	}
#endif

	return nullptr;
}

#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
FProperty *ue_struct_get_field_from_name(UScriptStruct *u_struct, char *name)
#else
UProperty *ue_struct_get_field_from_name(UScriptStruct *u_struct, char *name)
#endif
{
	return get_field_from_name(u_struct, name);
}

static PyObject *ue_PyUScriptStruct_getattro(ue_PyUScriptStruct *self, PyObject *attr_name)
{
	PyObject *ret = PyObject_GenericGetAttr((PyObject *)self, attr_name);
	if (!ret)
	{
		if (PyUnicodeOrString_Check(attr_name))
		{
			const char *attr = UEPyUnicode_AsUTF8(attr_name);
			// first check for property
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
			FProperty *f_property = get_field_from_name(self->u_struct, (char *)attr);
			if (f_property)
			{
				// swallow previous exception
				PyErr_Clear();
				return ue_py_convert_property(f_property, self->u_struct_ptr, 0);
			}
#else
			UProperty *u_property = get_field_from_name(self->u_struct, (char *)attr);
			if (u_property)
			{
				// swallow previous exception
				PyErr_Clear();
				return ue_py_convert_property(u_property, self->u_struct_ptr, 0);
			}
#endif
		}
	}
	return ret;
}

static int ue_PyUScriptStruct_setattro(ue_PyUScriptStruct *self, PyObject *attr_name, PyObject *value)
{
	// first of all check for Property (FProperty or UProperty)
	if (PyUnicodeOrString_Check(attr_name))
	{
		const char *attr = UEPyUnicode_AsUTF8(attr_name);
		// first check for property
#if ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 25)
		FProperty *f_property = get_field_from_name(self->u_struct, (char *)attr);
		if (f_property)
		{
			if (ue_py_convert_pyobject(value, f_property, self->u_struct_ptr, 0))
			{
				return 0;
			}
			PyErr_SetString(PyExc_ValueError, "invalid value for FProperty");
			return -1;
		}
#else
		UProperty *u_property = get_field_from_name(self->u_struct, (char *)attr);
		if (u_property)
		{
			if (ue_py_convert_pyobject(value, u_property, self->u_struct_ptr, 0))
			{
				return 0;
			}
			PyErr_SetString(PyExc_ValueError, "invalid value for UProperty");
			return -1;
		}
#endif
	}
	return PyObject_GenericSetAttr((PyObject *)self, attr_name, value);
}



// destructor
static void ue_PyUScriptStruct_dealloc(ue_PyUScriptStruct *self)
{
#if defined(UEPY_MEMORY_DEBUG)
	UE_LOG(LogPython, Warning, TEXT("Destroying ue_PyUScriptStruct %p with size %d"), self, self->u_struct->GetStructureSize());
#endif
	if (self->u_struct_owned)
	{
		FMemory::Free(self->u_struct_ptr);
	}
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

static int ue_py_uscriptstruct_init(ue_PyUScriptStruct *self, PyObject *args, PyObject *kwargs)
{
	PyObject *py_struct;
	if (!PyArg_ParseTuple(args, "O", &py_struct))
		return -1;

	if (!ue_is_pyuobject(py_struct))
	{
		PyErr_SetString(PyExc_Exception, "argument is not a UScriptStruct");
		return -1;
	}

	ue_PyUObject *py_u_obj = (ue_PyUObject *)py_struct;
	if (!py_u_obj->ue_object->IsA<UScriptStruct>())
	{
		PyErr_SetString(PyExc_Exception, "argument is not a UScriptStruct");
		return -1;
	}

	self->u_struct = (UScriptStruct *)py_u_obj->ue_object;
	self->u_struct_ptr = (uint8*)FMemory::Malloc(self->u_struct->GetStructureSize());
	self->u_struct->InitializeStruct(self->u_struct_ptr);
#if WITH_EDITOR
	self->u_struct->InitializeDefaultValue(self->u_struct_ptr);
#endif
	self->u_struct_owned = 1;
	return 0;
}

// get the original pointer of a struct (dumb function for backward compatibility with older scripts from
// a dark age where strctures were passed by value)
static PyObject *py_ue_uscriptstruct_ref(ue_PyUScriptStruct *self, PyObject * args)
{
	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *ue_py_uscriptstruct_richcompare(ue_PyUScriptStruct *u_struct1, PyObject *py_obj, int op)
{
	ue_PyUScriptStruct *u_struct2 = py_ue_is_uscriptstruct(py_obj);
	if (!u_struct2 || (op != Py_EQ && op != Py_NE))
	{
		return PyErr_Format(PyExc_NotImplementedError, "can only compare with another UScriptStruct");
	}

	bool equals = (u_struct1->u_struct == u_struct2->u_struct && !memcmp(u_struct1->u_struct_ptr, u_struct2->u_struct_ptr, u_struct1->u_struct->GetStructureSize()));

	if (op == Py_EQ)
	{
		if (equals)
		{
			Py_RETURN_TRUE;
		}
		Py_RETURN_FALSE;
	}

	if (equals)
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}




void ue_python_init_uscriptstruct(PyObject *ue_module)
{
	ue_PyUScriptStructType.tp_new = PyType_GenericNew;

	ue_PyUScriptStructType.tp_richcompare = (richcmpfunc)ue_py_uscriptstruct_richcompare;

	ue_PyUScriptStructType.tp_init = (initproc)ue_py_uscriptstruct_init;

	if (PyType_Ready(&ue_PyUScriptStructType) < 0)
		return;

	Py_INCREF(&ue_PyUScriptStructType);
	PyModule_AddObject(ue_module, "UScriptStruct", (PyObject *)&ue_PyUScriptStructType);
}

PyObject *py_ue_new_uscriptstruct(UScriptStruct *u_struct, uint8 *data)
{
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = u_struct;
	ret->u_struct_ptr = data;
	ret->u_struct_owned = 0;
	return (PyObject *)ret;
}

PyObject *py_ue_new_owned_uscriptstruct(UScriptStruct *u_struct, uint8 *data)
{
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = u_struct;
	uint8 *struct_data = (uint8*)FMemory::Malloc(u_struct->GetStructureSize());
	ret->u_struct->InitializeStruct(struct_data);
	ret->u_struct->CopyScriptStruct(struct_data, data);
	ret->u_struct_ptr = struct_data;
	ret->u_struct_owned = 1;
	return (PyObject *)ret;
}

PyObject *py_ue_new_owned_uscriptstruct_zero_copy(UScriptStruct *u_struct, uint8 *data)
{
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = u_struct;
	ret->u_struct_ptr = data;
	ret->u_struct_owned = 1;
	return (PyObject *)ret;
}

static PyObject *py_ue_uscriptstruct_clone(ue_PyUScriptStruct *self, PyObject * args)
{
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = self->u_struct;
	uint8 *struct_data = (uint8*)FMemory::Malloc(self->u_struct->GetStructureSize());
	ret->u_struct->InitializeStruct(struct_data);
	ret->u_struct->CopyScriptStruct(struct_data, self->u_struct_ptr);
	ret->u_struct_ptr = struct_data;
	ret->u_struct_owned = 1;
	return (PyObject *)ret;
}

ue_PyUScriptStruct *py_ue_is_uscriptstruct(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUScriptStructType))
		return nullptr;
	return (ue_PyUScriptStruct *)obj;
}
