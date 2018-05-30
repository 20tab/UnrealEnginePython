#include "UEPyFbxManager.h"

#if ENGINE_MINOR_VERSION > 12

#if WITH_EDITOR

#include "UEPyFbx.h"

static PyObject *py_ue_fbx_manager_set_io_settings(ue_PyFbxManager *self, PyObject *args)
{
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object))
	{
		return nullptr;
	}

	ue_PyFbxIOSettings *py_fbx_io_settings = py_ue_is_fbx_io_settings(py_object);
	if (!py_fbx_io_settings)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FbxIOSettings");
	}

	self->fbx_manager->SetIOSettings(py_fbx_io_settings->fbx_io_settings);

	Py_RETURN_NONE;
}

static PyObject *py_ue_fbx_manager_create_missing_bind_poses(ue_PyFbxManager *self, PyObject *args)
{
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object))
	{
		return nullptr;
	}

	ue_PyFbxScene *py_fbx_scene = py_ue_is_fbx_scene(py_object);
	if (!py_fbx_scene)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FbxScene");
	}

	self->fbx_manager->CreateMissingBindPoses(py_fbx_scene->fbx_scene);

	Py_RETURN_NONE;
}


static PyMethodDef ue_PyFbxManager_methods[] = {
	{ "set_io_settings", (PyCFunction)py_ue_fbx_manager_set_io_settings, METH_VARARGS, "" },
	{ "create_missing_bind_poses", (PyCFunction)py_ue_fbx_manager_create_missing_bind_poses, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static void ue_py_fbx_manager_dealloc(ue_PyFbxManager *self)
{
	if (self->fbx_manager)
		self->fbx_manager->Destroy();
#if PY_MAJOR_VERSION < 3
	self->ob_type->tp_free((PyObject*)self);
#else
	Py_TYPE(self)->tp_free((PyObject*)self);
#endif
}

static PyTypeObject ue_PyFbxManagerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxManager", /* tp_name */
	sizeof(ue_PyFbxManager),    /* tp_basicsize */
	0,                         /* tp_itemsize */
	(destructor)ue_py_fbx_manager_dealloc,   /* tp_dealloc */
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
	"Unreal Engine FbxManager", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxManager_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_manager_init(ue_PyFbxManager *self, PyObject * args)
{
	self->fbx_manager = FbxManager::Create();
	return 0;
}

void ue_python_init_fbx_manager(PyObject *ue_module)
{
	ue_PyFbxManagerType.tp_new = PyType_GenericNew;;
	ue_PyFbxManagerType.tp_init = (initproc)py_ue_fbx_manager_init;
	if (PyType_Ready(&ue_PyFbxManagerType) < 0)
		return;

	Py_INCREF(&ue_PyFbxManagerType);
	PyModule_AddObject(ue_module, "FbxManager", (PyObject *)&ue_PyFbxManagerType);
}


ue_PyFbxManager *py_ue_is_fbx_manager(PyObject *obj)
{
	if (!PyObject_IsInstance(obj, (PyObject *)&ue_PyFbxManagerType))
		return nullptr;
	return (ue_PyFbxManager *)obj;
}

void ue_python_init_fbx(PyObject *module)
{
	ue_python_init_fbx_manager(module);
	ue_python_init_fbx_io_settings(module);
	ue_python_init_fbx_importer(module);
	ue_python_init_fbx_scene(module);
	ue_python_init_fbx_node(module);
	ue_python_init_fbx_object(module);
	ue_python_init_fbx_property(module);
	ue_python_init_fbx_pose(module);
	ue_python_init_fbx_mesh(module);
}


#endif
#endif
