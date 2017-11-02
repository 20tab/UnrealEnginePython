#if WITH_EDITOR
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPyICollectionManager.h"

#include "Developer/CollectionManager/Public/CollectionManagerModule.h"

static PyObject *py_ue_icollection_manager_get_collections(PyObject *cls, PyObject * args)
{
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FCollectionNameType> collections;
	CollectionManager.GetCollections(collections);
	for (FCollectionNameType name_type : collections) {
		PyList_Append(py_list, Py_BuildValue((char *)"(si)", TCHAR_TO_UTF8(*name_type.Name.ToString()), (int)name_type.Type));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_create_static_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.CreateCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, ECollectionStorageMode::Static))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_create_dynamic_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.CreateCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, ECollectionStorageMode::Dynamic))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_reparent_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	char *parent;
	int parent_type;

	if (!PyArg_ParseTuple(args, "sisi", &name, &type, &parent, &parent_type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.ReparentCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		FName(UTF8_TO_TCHAR(parent)),
		(ECollectionShareType::Type)parent_type
	))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}


static PyMethodDef ue_PyICollectionManager_methods[] = {
	{ "get_collections", (PyCFunction)py_ue_icollection_manager_get_collections, METH_VARARGS | METH_CLASS, "" },
	{ "create_static_collection", (PyCFunction)py_ue_icollection_manager_create_static_collection, METH_VARARGS | METH_CLASS, "" },
	{ "create_dynamic_collection", (PyCFunction)py_ue_icollection_manager_create_dynamic_collection, METH_VARARGS | METH_CLASS, "" },
	{ "reparent_collection", (PyCFunction)py_ue_icollection_manager_reparent_collection, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyICollectionManagerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.ICollectionManager", /* tp_name */
	sizeof(ue_PyICollectionManager), /* tp_basicsize */
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
	0,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT,        /* tp_flags */
	"Unreal Engine CollectionManager Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyICollectionManager_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_icollection_manager_init(ue_PyICollectionManager *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "ICollectionManager is a singleton");
	return -1;
}

void ue_python_init_icollection_manager(PyObject *ue_module)
{
	ue_PyICollectionManagerType.tp_new = PyType_GenericNew;
	ue_PyICollectionManagerType.tp_init = (initproc)py_ue_icollection_manager_init;

	if (PyType_Ready(&ue_PyICollectionManagerType) < 0)
		return;

	Py_INCREF(&ue_PyICollectionManagerType);
	PyModule_AddObject(ue_module, "ICollectionManager", (PyObject *)&ue_PyICollectionManagerType);
}

#endif
