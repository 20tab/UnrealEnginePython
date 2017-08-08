
#if ENGINE_MINOR_VERSION > 12
#include "UnrealEnginePythonPrivatePCH.h"

#if WITH_EDITOR

#include "UEPyFbx.h"

static PyObject *py_ue_fbx_importer_initialize(ue_PyFbxImporter *self, PyObject *args) {
	char *filename;
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "sO", &filename, &py_object)) {
		return nullptr;
	}

	ue_PyFbxIOSettings *py_fbx_io_settings = py_ue_is_fbx_io_settings(py_object);
	if (!py_fbx_io_settings) {
		return PyErr_Format(PyExc_Exception, "argument is not a FbxIOSettings");
	}

	if (self->fbx_importer->Initialize(filename, -1, py_fbx_io_settings->fbx_io_settings)) {
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyObject *py_ue_fbx_importer_import(ue_PyFbxImporter *self, PyObject *args) {
	PyObject *py_object;
	if (!PyArg_ParseTuple(args, "O", &py_object)) {
		return nullptr;
	}

	ue_PyFbxScene *py_fbx_scene = py_ue_is_fbx_scene(py_object);
	if (!py_fbx_scene) {
		return PyErr_Format(PyExc_Exception, "argument is not a FbxScene");
	}

	if (self->fbx_importer->Import(py_fbx_scene->fbx_scene)) {
		Py_RETURN_TRUE;
	}

	Py_RETURN_FALSE;
}

static PyMethodDef ue_PyFbxImporter_methods[] = {
	{ "initialize", (PyCFunction)py_ue_fbx_importer_initialize, METH_VARARGS, "" },
	{ "_import", (PyCFunction)py_ue_fbx_importer_import, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

static PyTypeObject ue_PyFbxImporterType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.FbxImporter", /* tp_name */
	sizeof(ue_PyFbxImporter),    /* tp_basicsize */
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
	"Unreal Engine FbxImporter", /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyFbxImporter_methods,    /* tp_methods */
	0,   /* tp_members */
	0,                         /* tp_getset */
};

static int py_ue_fbx_importer_init(ue_PyFbxImporter *self, PyObject * args) {
	PyObject *py_object;
	char *name;
	if (!PyArg_ParseTuple(args, "Os", &py_object, &name)) {
		return -1;
	}

	ue_PyFbxManager *py_fbx_manager = py_ue_is_fbx_manager(py_object);
	if (!py_fbx_manager) {
		PyErr_SetString(PyExc_Exception, "argument is not a FbxManager");
		return -1;
	}

	self->fbx_importer = FbxImporter::Create(py_fbx_manager->fbx_manager, name);
	return 0;
}

void ue_python_init_fbx_importer(PyObject *ue_module) {
	ue_PyFbxImporterType.tp_new = PyType_GenericNew;;
	ue_PyFbxImporterType.tp_init = (initproc)py_ue_fbx_importer_init;
	if (PyType_Ready(&ue_PyFbxImporterType) < 0)
		return;

	Py_INCREF(&ue_PyFbxImporterType);
	PyModule_AddObject(ue_module, "FbxImporter", (PyObject *)&ue_PyFbxImporterType);
}

#endif
#endif
