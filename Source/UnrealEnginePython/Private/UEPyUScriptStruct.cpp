

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

static PyMethodDef ue_PyUScriptStruct_methods[] = {
	{ "get_field", (PyCFunction)py_ue_uscriptstruct_get_field, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};


static PyObject *ue_PyUScriptStruct_str(ue_PyUScriptStruct *self)
{
	return PyUnicode_FromFormat("<unreal_engine.UScriptStruct {'struct': '%s', 'size': %d}>",
		TCHAR_TO_UTF8(*self->u_struct->GetName()), self->u_struct->GetStructureSize());
}

static PyTypeObject ue_PyUScriptStructType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.UScriptStruct", /* tp_name */
	sizeof(ue_PyUScriptStruct), /* tp_basicsize */
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
	(reprfunc)ue_PyUScriptStruct_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
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


void ue_python_init_uscriptstruct(PyObject *ue_module) {
	ue_PyUScriptStructType.tp_new = PyType_GenericNew;

	if (PyType_Ready(&ue_PyUScriptStructType) < 0)
		return;

	Py_INCREF(&ue_PyUScriptStructType);
	PyModule_AddObject(ue_module, "UScriptStruct", (PyObject *)&ue_PyUScriptStructType);
}

PyObject *py_ue_new_uscriptstruct(UScriptStruct *u_struct, uint8 *data) {
	ue_PyUScriptStruct *ret = (ue_PyUScriptStruct *)PyObject_New(ue_PyUScriptStruct, &ue_PyUScriptStructType);
	ret->u_struct = u_struct;
	ret->data = data;
	return (PyObject *)ret;
}

ue_PyUScriptStruct *py_ue_is_uscriptstruct(PyObject *obj) {
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyUScriptStructType))
		return nullptr;
	return (ue_PyUScriptStruct *)obj;
}