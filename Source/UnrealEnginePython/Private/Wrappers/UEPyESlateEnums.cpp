#include "UEPyESlateEnums.h"

#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"

static PyObject *py_ue_eslate_enums_get(ue_PyESlateEnums *self, void *closure)
{
	return PyLong_FromLong(self->val);
}

static PyGetSetDef ue_PyESlateEnums_getseters[] = {
	{ (char*)"val", (getter)py_ue_eslate_enums_get, 0, (char *)"", NULL },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyESlateEnums_str(ue_PyESlateEnums *self)
{
	return PyUnicode_FromFormat("<unreal_engine.ESlateEnums {'val': %S}>", PyLong_FromLong(self->val));
}

static PyTypeObject ue_PyESlateEnumsType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.ESlateEnums", /* tp_name */
	sizeof(ue_PyESlateEnums), /* tp_basicsize */
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
	(reprfunc)ue_PyESlateEnums_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine ESlateEnums",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	0,                         /* tp_methods */
	0,
	ue_PyESlateEnums_getseters,
};

static int ue_py_eslate_enums_init(ue_PyESlateEnums *self, PyObject *args, PyObject *kwargs)
{
	int val = 0;
	if (!PyArg_ParseTuple(args, "i", &val))
		return -1;

	self->val = val;
	return 0;
}

void ue_python_init_eslate_enums(PyObject *ue_module)
{
	ue_PyESlateEnumsType.tp_new = PyType_GenericNew;

	ue_PyESlateEnumsType.tp_init = (initproc)ue_py_eslate_enums_init;

	if (PyType_Ready(&ue_PyESlateEnumsType) < 0)
		return;

	Py_INCREF(&ue_PyESlateEnumsType);
	PyModule_AddObject(ue_module, "ESlateEnums", (PyObject *)&ue_PyESlateEnumsType);

	auto add_native_enum = [](const char *enum_name, uint8 val)
	{
		ue_PyESlateEnums* native_enum = (ue_PyESlateEnums *)PyObject_New(ue_PyESlateEnums, &ue_PyESlateEnumsType);
		native_enum->val = val;
		PyDict_SetItemString(ue_PyESlateEnumsType.tp_dict, enum_name, (PyObject *)native_enum);
	};

#if ENGINE_MINOR_VERSION > 15
#if ENGINE_MINOR_VERSION >= 23
#define ADD_NATIVE_ENUM(EnumType, EnumVal) add_native_enum(#EnumType "." #EnumVal, (uint8)EnumType::EnumVal)
#else
#define ADD_NATIVE_ENUM(EnumType, EnumVal) add_native_enum(#EnumType "." #EnumVal, (uint8)EnumType::Type::EnumVal)
#endif
	ADD_NATIVE_ENUM(EUserInterfaceActionType, None);
	ADD_NATIVE_ENUM(EUserInterfaceActionType, Button);
	ADD_NATIVE_ENUM(EUserInterfaceActionType, ToggleButton);
	ADD_NATIVE_ENUM(EUserInterfaceActionType, RadioButton);
	ADD_NATIVE_ENUM(EUserInterfaceActionType, Check);
	ADD_NATIVE_ENUM(EUserInterfaceActionType, CollapsedButton);
#undef ADD_NATIVE_ENUM
#endif

}

ue_PyESlateEnums *py_ue_is_eslate_enums(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyESlateEnumsType))
		return nullptr;
	return (ue_PyESlateEnums *)obj;

}
