
#if WITH_EDITOR


#include "UnrealEnginePythonPrivatePCH.h"
#include "Runtime/Engine/Classes/EdGraph/EdGraphPin.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"

static PyObject *py_ue_edgraphpin_make_link_to(ue_PyEdGraphPin *self, PyObject * args) {
	PyObject *other_pin;
	if (!PyArg_ParseTuple(args, "O:make_link_to", &other_pin)) {
		return NULL;
	}

	ue_PyEdGraphPin *py_other_pin = py_ue_is_edgraphpin(other_pin);
	if (!py_other_pin) {
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraphPin");
	}

	self->pin->MakeLinkTo(py_other_pin->pin);

	UBlueprint *bp = (UBlueprint *)self->pin->GetOwningNode()->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_edgraphpin_break_link_to(ue_PyEdGraphPin *self, PyObject * args) {
	PyObject *other_pin;
	if (!PyArg_ParseTuple(args, "O:break_link_to", &other_pin)) {
		return NULL;
	}

	ue_PyEdGraphPin *py_other_pin = py_ue_is_edgraphpin(other_pin);
	if (!py_other_pin) {
		return PyErr_Format(PyExc_Exception, "argument is not a UEdGraphPin");
	}

	self->pin->BreakLinkTo(py_other_pin->pin);

	UBlueprint *bp = (UBlueprint *)self->pin->GetOwningNode()->GetGraph()->GetOuter();
	FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(bp);

	Py_INCREF(Py_None);
	return Py_None;
}

static PyMethodDef ue_PyEdGraphPin_methods[] = {
	{ "make_link_to", (PyCFunction)py_ue_edgraphpin_make_link_to, METH_VARARGS, "" },
	{ "break_link_to", (PyCFunction)py_ue_edgraphpin_break_link_to, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *py_ue_edgraphpin_get_name(ue_PyEdGraphPin *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinName)));
}

static PyObject *py_ue_edgraphpin_get_category(ue_PyEdGraphPin *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinCategory)));
}

static PyObject *py_ue_edgraphpin_get_sub_category(ue_PyEdGraphPin *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->PinType.PinSubCategory)));
}

static PyObject *py_ue_edgraphpin_get_default_value(ue_PyEdGraphPin *self, void *closure) {
	return PyUnicode_FromString(TCHAR_TO_UTF8(*(self->pin->DefaultValue)));
}

static int py_ue_edgraphpin_set_default_value(ue_PyEdGraphPin *self, PyObject *value, void *closure) {
	if (value && PyUnicode_Check(value)) {
		char *str = PyUnicode_AsUTF8(value);
		self->pin->DefaultValue = UTF8_TO_TCHAR(str);
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a string");
	return -1;
}

static PyObject *py_ue_edgraphpin_get_default_object(ue_PyEdGraphPin *self, void *closure) {
	UObject *u_object = self->pin->DefaultObject;
	if (!u_object) {
		Py_INCREF(Py_None);
		return Py_None;
	}
	PyObject *ret = (PyObject *)ue_get_python_wrapper(u_object);
	if (!ret)
		return PyErr_Format(PyExc_Exception, "uobject is in invalid state");
	Py_INCREF(ret);
	return ret;
}

static int py_ue_edgraphpin_set_default_object(ue_PyEdGraphPin *self, PyObject *value, void *closure) {
	if (value && ue_is_pyuobject(value)) {
		ue_PyUObject *py_obj = (ue_PyUObject *)value;
		self->pin->DefaultObject = py_obj->ue_object;
		return 0;
	}
	PyErr_SetString(PyExc_TypeError, "value is not a UObject");
	return -1;
}

static PyGetSetDef ue_PyEdGraphPin_getseters[] = {
	{ (char*)"name", (getter)py_ue_edgraphpin_get_name, NULL, (char *)"", NULL },
	{ (char*)"category", (getter)py_ue_edgraphpin_get_category, NULL, (char *)"", NULL },
	{ (char*)"sub_category", (getter)py_ue_edgraphpin_get_sub_category, NULL, (char *)"", NULL },
	{ (char*)"default_value", (getter)py_ue_edgraphpin_get_default_value, (setter)py_ue_edgraphpin_set_default_value, (char *)"", NULL },
	{ (char*)"default_object", (getter)py_ue_edgraphpin_get_default_object, (setter)py_ue_edgraphpin_set_default_object, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyEdGraphPin_str(ue_PyEdGraphPin *self)
{
	return PyUnicode_FromFormat("<unreal_engine.EdGraphPin {'name': '%s', 'type': '%s'}>",
		TCHAR_TO_UTF8(*self->pin->PinName), TCHAR_TO_UTF8(*self->pin->PinType.PinCategory));
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


void ue_python_init_edgraphpin(PyObject *ue_module) {
	ue_PyEdGraphPinType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyEdGraphPinType) < 0)
		return;

	Py_INCREF(&ue_PyEdGraphPinType);
	PyModule_AddObject(ue_module, "EdGraphPin", (PyObject *)&ue_PyEdGraphPinType);
}

PyObject *py_ue_new_edgraphpin(UEdGraphPin *pin) {
	ue_PyEdGraphPin *ret = (ue_PyEdGraphPin *)PyObject_New(ue_PyEdGraphPin, &ue_PyEdGraphPinType);
	ret->pin = pin;
	return (PyObject *)ret;
}

ue_PyEdGraphPin *py_ue_is_edgraphpin(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyEdGraphPinType))
		return nullptr;
	return (ue_PyEdGraphPin *)obj;
}

#endif