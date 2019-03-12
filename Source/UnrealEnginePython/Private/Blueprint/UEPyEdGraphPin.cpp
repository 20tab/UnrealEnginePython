#include "UEPyEdGraphPin.h"

#if WITH_EDITOR

#include "Runtime/Engine/Classes/EdGraph/EdGraphPin.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

static PyObject *py_ue_edgraphpin_make_link_to(ue_PyEdGraphPin *self, PyObject * args)
{
	PyObject *other_pin;
	if (!PyArg_ParseTuple(args, "O:make_link_to", &other_pin))
	{
		return NULL;
	}

	ue_PyEdGraphPin *py_other_pin = py_ue_is_edgraphpin(other_pin);
	if (!py_other_pin)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraphPin");
	}

	self->pin->MakeLinkTo(py_other_pin->pin);

	if (UBlueprint *bp = Cast<UBlueprint>(self->pin->GetOwningNode()->GetGraph()->GetOuter()))
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_edgraphpin_connect(ue_PyEdGraphPin *self, PyObject * args)
{
	PyObject *other_pin;
	if (!PyArg_ParseTuple(args, "O:connect", &other_pin))
	{
		return NULL;
	}

	ue_PyEdGraphPin *py_other_pin = py_ue_is_edgraphpin(other_pin);
	if (!py_other_pin)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraphPin");
	}

	if (!self->pin->GetSchema()->TryCreateConnection(self->pin, py_other_pin->pin))
	{
		return PyErr_Format(PyExc_Exception, "unable to connect pins");
	}

	if (UBlueprint *bp = Cast<UBlueprint>(self->pin->GetOwningNode()->GetGraph()->GetOuter()))
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_edgraphpin_break_link_to(ue_PyEdGraphPin *self, PyObject * args)
{
	PyObject *other_pin;
	if (!PyArg_ParseTuple(args, "O:break_link_to", &other_pin))
	{
		return NULL;
	}

	ue_PyEdGraphPin *py_other_pin = py_ue_is_edgraphpin(other_pin);
	if (!py_other_pin)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraphPin");
	}

	self->pin->BreakLinkTo(py_other_pin->pin);

	if (UBlueprint *bp = Cast<UBlueprint>(self->pin->GetOwningNode()->GetGraph()->GetOuter()))
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_edgraphpin_break_all_pin_links(ue_PyEdGraphPin *self, PyObject * args)
{
	PyObject *py_notify_nodes = nullptr;
	if (!PyArg_ParseTuple(args, "O:break_all_pin_links", &py_notify_nodes))
	{
		return NULL;
	}

	bool notify_nodes = true;
	if (py_notify_nodes && !PyObject_IsTrue(py_notify_nodes))
		notify_nodes = false;

	self->pin->BreakAllPinLinks(notify_nodes);

	if (UBlueprint *bp = Cast<UBlueprint>(self->pin->GetOwningNode()->GetGraph()->GetOuter()))
	{
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_edgraphpin_get_linked_to(ue_PyEdGraphPin * self, PyObject * args)
{
	PyObject *pins = PyList_New(0);

	TArray<UEdGraphPin*> Links = self->pin->LinkedTo;

	for (int32 i = 0; i < Links.Num(); i++)
	{
		UEdGraphPin *pin = Links[i];
		ue_PyUObject *item = (ue_PyUObject *)py_ue_new_edgraphpin(pin);
		if (item)
			PyList_Append(pins, (PyObject *)item);
	}
	return pins;
}

static PyMethodDef ue_PyEdGraphPin_methods[] = {
	{ "make_link_to", (PyCFunction)py_ue_edgraphpin_make_link_to, METH_VARARGS, "" },
	{ "break_link_to", (PyCFunction)py_ue_edgraphpin_break_link_to, METH_VARARGS, "" },
	{ "break_all_pin_links", (PyCFunction)py_ue_edgraphpin_break_all_pin_links, METH_VARARGS, "" },
	{ "get_linked_to", (PyCFunction)py_ue_edgraphpin_get_linked_to, METH_VARARGS, "" },
	{ "connect", (PyCFunction)py_ue_edgraphpin_connect, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_edgraphpin_get_name(ue_PyEdGraphPin *self, void *closure)
{
#if ENGINE_MINOR_VERSION > 18
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinName.ToString())));
#else
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinName)));
#endif
}

static PyObject *py_ue_edgraphpin_get_category(ue_PyEdGraphPin *self, void *closure)
{
#if ENGINE_MINOR_VERSION > 18
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinCategory.ToString())));
#else
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinCategory)));
#endif
}

static PyObject *py_ue_edgraphpin_get_sub_category(ue_PyEdGraphPin *self, void *closure)
{
#if ENGINE_MINOR_VERSION > 18
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinSubCategory.ToString())));
#else
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinSubCategory)));
#endif
}

static PyObject *py_ue_edgraphpin_get_default_value(ue_PyEdGraphPin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->DefaultValue)));
}

static int py_ue_edgraphpin_set_default_value(ue_PyEdGraphPin *self, PyObject *value, void *closure)
{
	if (value && PyUnicodeOrString_Check(value))
	{
		const char *str = UEPyUnicode_AsUTF8(value);
		self->pin->DefaultValue = UTF8_TO_TCHAR(str);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a string");
	return -1;
}

static PyObject *py_ue_edgraphpin_get_default_text_value(ue_PyEdGraphPin *self, void *closure)
{
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->DefaultTextValue.ToString())));
}

static int py_ue_edgraphpin_set_default_text_value(ue_PyEdGraphPin *self, PyObject *value, void *closure)
{
	if (value && PyUnicodeOrString_Check(value))
	{
		const char *str = UEPyUnicode_AsUTF8(value);
		self->pin->DefaultTextValue = FText::FromString(UTF8_TO_TCHAR(str));
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a string");
	return -1;
}



static PyObject *py_ue_edgraphpin_get_default_object(ue_PyEdGraphPin *self, void *closure)
{
	UObject *u_object = self->pin->DefaultObject;
	if (!u_object)
	{
		Py_RETURN_NONE;
	}
	Py_RETURN_UOBJECT(u_object);
}

static int py_ue_edgraphpin_set_default_object(ue_PyEdGraphPin *self, PyObject *value, void *closure)
{
	if (value && ue_is_pyuobject(value))
	{
		ue_PyUObject *py_obj = (ue_PyUObject *)value;
		self->pin->DefaultObject = py_obj->ue_object;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a UObject");
	return -1;
}

static int py_ue_edgraphpin_set_category(ue_PyEdGraphPin *self, PyObject *value, void *closure)
{
	if (value && PyUnicodeOrString_Check(value))
	{
		const char *str = UEPyUnicode_AsUTF8(value);
#if ENGINE_MINOR_VERSION > 18
		self->pin->PinType.PinCategory = FName(UTF8_TO_TCHAR(str));
#else
		self->pin->PinType.PinCategory = FString(UTF8_TO_TCHAR(str));
#endif
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a string");
	return -1;
}

static int py_ue_edgraphpin_set_sub_category(ue_PyEdGraphPin *self, PyObject *value, void *closure)
{
	if (value)
	{
		if (ue_is_pyuobject(value))
		{
			ue_PyUObject *py_obj = (ue_PyUObject *)value;
			self->pin->PinType.PinSubCategoryObject = py_obj->ue_object;
			return 0;
		}
		if (PyUnicodeOrString_Check(value))
		{
			const char *str = UEPyUnicode_AsUTF8(value);
#if ENGINE_MINOR_VERSION > 18
			self->pin->PinType.PinSubCategory = FName(UTF8_TO_TCHAR(str));
#else
			self->pin->PinType.PinSubCategory = FString(UTF8_TO_TCHAR(str));
#endif
			return 0;
		}
	}
	PyErr_SetString(PyExc_TypeError, "value is not a UObject");
	return -1;
}

static PyGetSetDef ue_PyEdGraphPin_getseters[] = {
	{ (char*)"name", (getter)py_ue_edgraphpin_get_name, NULL, (char *)"", NULL },
	{ (char*)"category", (getter)py_ue_edgraphpin_get_category, (setter)py_ue_edgraphpin_set_category, (char *)"", NULL },
	{ (char*)"sub_category", (getter)py_ue_edgraphpin_get_sub_category, (setter)py_ue_edgraphpin_set_sub_category, (char *)"", NULL },
	{ (char*)"default_value", (getter)py_ue_edgraphpin_get_default_value, (setter)py_ue_edgraphpin_set_default_value, (char *)"", NULL },
	{ (char*)"default_text_value", (getter)py_ue_edgraphpin_get_default_text_value, (setter)py_ue_edgraphpin_set_default_text_value, (char *)"", NULL },
	{ (char*)"default_object", (getter)py_ue_edgraphpin_get_default_object, (setter)py_ue_edgraphpin_set_default_object, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyEdGraphPin_str(ue_PyEdGraphPin *self)
{
	return PyUnicode_FromFormat("<unreal_engine.EdGraphPin {'name': '%s', 'type': '%s'}>",
#if ENGINE_MINOR_VERSION > 18
		TCHAR_TO_UTF8(*self->pin->PinName.ToString()), TCHAR_TO_UTF8(*self->pin->PinType.PinCategory.ToString()));
#else
		TCHAR_TO_UTF8(*self->pin->PinName), TCHAR_TO_UTF8(*self->pin->PinType.PinCategory));
#endif
}

static PyTypeObject ue_PyEdGraphPinType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.EdGraphPin", /* tp_name */
	sizeof(ue_PyEdGraphPin), /* tp_basicsize */
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
	(reprfunc)ue_PyEdGraphPin_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine Editor GraphPin",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyEdGraphPin_methods,             /* tp_methods */
	0,
	ue_PyEdGraphPin_getseters,
};


void ue_python_init_edgraphpin(PyObject *ue_module)
{
	ue_PyEdGraphPinType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyEdGraphPinType) < 0)
		return;

	Py_INCREF(&ue_PyEdGraphPinType);
	PyModule_AddObject(ue_module, "EdGraphPin", (PyObject *)&ue_PyEdGraphPinType);
}

PyObject *py_ue_new_edgraphpin(UEdGraphPin *pin)
{
	ue_PyEdGraphPin *ret = (ue_PyEdGraphPin *)PyObject_New(ue_PyEdGraphPin, &ue_PyEdGraphPinType);
	ret->pin = pin;
	return (PyObject *)ret;
}

ue_PyEdGraphPin *py_ue_is_edgraphpin(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyEdGraphPinType))
		return nullptr;
	return (ue_PyEdGraphPin *)obj;
}

#endif
