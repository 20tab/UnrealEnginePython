#include "UEPyFModifierKeysState.h"

static PyObject *py_ue_fmodifier_keys_state_are_caps_locked(ue_PyFModifierKeysState *self, PyObject * args)
{
	if (self->modifier.AreCapsLocked())
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

static PyMethodDef ue_PyFModifierKeysState_methods[] = {
	{ "are_caps_locked", (PyCFunction)py_ue_fmodifier_keys_state_are_caps_locked, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyObject *ue_PyFModifierKeysState_str(ue_PyFModifierKeysState *self)
{
	return PyUnicode_FromFormat("<unreal_engine.FModifierKeysState %p>", self);
}

static PyTypeObject ue_PyFModifierKeysStateType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FModifierKeysState", /* tp_name */
	sizeof(ue_PyFModifierKeysState), /* tp_basicsize */
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
	(reprfunc)ue_PyFModifierKeysState_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine FModifierKeysState",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFModifierKeysState_methods,             /* tp_methods */
};

static int ue_py_fmodifier_keys_state_init(ue_PyFModifierKeysState *self, PyObject *args, PyObject *kwargs)
{
	static char *kw_names[] = {
		(char *)"left_shift_down",
		(char *)"right_shift_down",
		(char *)"left_control_down",
		(char *)"right_control_down",
		(char *)"left_alt_down",
		(char *)"right_alt_down",
		(char *)"left_command_down",
		(char *)"right_command_down",
		(char *)"are_caps_locked",
		NULL };

	PyObject *py_left_shift_down = nullptr;
	PyObject *py_right_shift_down = nullptr;
	PyObject *py_left_control_down = nullptr;
	PyObject *py_right_control_down = nullptr;
	PyObject *py_left_alt_down = nullptr;
	PyObject *py_right_alt_down = nullptr;
	PyObject *py_left_command_down = nullptr;
	PyObject *py_right_command_down = nullptr;
	PyObject *py_are_capse_locked = nullptr;

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOOOOOO", kw_names,
		&py_left_shift_down,
		&py_right_shift_down,
		&py_left_control_down,
		&py_right_control_down,
		&py_left_alt_down,
		&py_right_alt_down,
		&py_left_command_down,
		&py_right_command_down,
		&py_are_capse_locked))
	{
		return -1;
	}

	bool bInIsLeftShiftDown = (py_left_shift_down && PyObject_IsTrue(py_left_shift_down));
	bool bInIsRightShiftDown = (py_right_shift_down && PyObject_IsTrue(py_right_shift_down));
	bool bInIsLeftControlDown = (py_left_control_down && PyObject_IsTrue(py_left_control_down));
	bool bInIsRightControlDown = (py_right_control_down && PyObject_IsTrue(py_right_control_down));
	bool bInIsLeftAltDown = (py_left_alt_down && PyObject_IsTrue(py_left_alt_down));
	bool bInIsRightAltDown = (py_right_alt_down && PyObject_IsTrue(py_right_alt_down));
	bool bInIsLeftCommandDown = (py_left_command_down && PyObject_IsTrue(py_left_command_down));
	bool bInIsRightCommandDown = (py_right_command_down && PyObject_IsTrue(py_right_command_down));
	bool bInAreCapsLocked = (py_are_capse_locked && PyObject_IsTrue(py_are_capse_locked));

	new(&self->modifier) FModifierKeysState(
		bInIsLeftShiftDown,
		bInIsRightShiftDown,
		bInIsLeftControlDown,
		bInIsRightControlDown,
		bInIsLeftAltDown,
		bInIsRightAltDown,
		bInIsLeftCommandDown,
		bInIsRightCommandDown,
		bInAreCapsLocked);

	return 0;
}

void ue_python_init_fmodifier_keys_state(PyObject *ue_module)
{
	ue_PyFModifierKeysStateType.tp_new = PyType_GenericNew;
	ue_PyFModifierKeysStateType.tp_init = (initproc)ue_py_fmodifier_keys_state_init;

	if (PyType_Ready(&ue_PyFModifierKeysStateType) < 0)
		return;

	Py_INCREF(&ue_PyFModifierKeysStateType);
	PyModule_AddObject(ue_module, "FModifierKeysState", (PyObject *)&ue_PyFModifierKeysStateType);
}

PyObject *py_ue_new_fmodifier_keys_state(FModifierKeysState modifier)
{
	ue_PyFModifierKeysState *ret = (ue_PyFModifierKeysState *)PyObject_New(ue_PyFModifierKeysState, &ue_PyFModifierKeysStateType);
	new(&ret->modifier) FModifierKeysState(modifier);
	return (PyObject *)ret;
}

ue_PyFModifierKeysState *py_ue_is_fmodifier_keys_state(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFModifierKeysStateType))
		return nullptr;
	return (ue_PyFModifierKeysState *)obj;
}