#include "UEPyFbxIOSettings.h"

#if ENGINE_MINOR_VERSION > 12

#if WITH_EDITOR

#include "UEPyFbx.h"

static PyMethodDef ue_PyFbxIOSettings_methods[] = {
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxIOSettingsType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxIOSettings", /* tp_name */
	sizeof(ue_PyFbxIOSettings),    /* tp_basicsize */
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
	"Unreal Engine FbxIOSettings", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxIOSettings_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_io_settings_init(ue_PyFbxIOSettings *self, PyObject * args)
{
	PyObject *py_object;
	char *name;
	if (!PyArg_ParseTuple(args, "Os", &py_object, &name))
	{
		return -1;
	}

	ue_PyFbxManager *py_fbx_manager = py_ue_is_fbx_manager(py_object);
	if (!py_fbx_manager)
	{
		PyErr_SetString(PyExc_Exception, "argument is not a FbxManager");
		return -1;
	}

	self->fbx_io_settings = FbxIOSettings::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_io_settings(PyObject *ue_module)
{
	ue_PyFbxIOSettingsType.tp_new = PyType_GenericNew;;
	ue_PyFbxIOSettingsType.tp_init = (initproc)py_ue_fbx_io_settings_init;
	if (PyType_Ready(&ue_PyFbxIOSettingsType) < 0)
		return;

	Py_INCREF(&ue_PyFbxIOSettingsType);
	PyModule_AddObject(ue_module, "FbxIOSettings", (PyObject *)&ue_PyFbxIOSettingsType);
}

ue_PyFbxIOSettings *py_ue_is_fbx_io_settings(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFbxIOSettingsType))
		return nullptr;
	return (ue_PyFbxIOSettings *)obj;
}

#endif
#endif
