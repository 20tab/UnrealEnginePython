#include "UEPyICollectionManager.h"

#if WITH_EDITOR

#include "Developer/CollectionManager/Public/CollectionManagerModule.h"

static PyObject *py_ue_icollection_manager_get_collections(PyObject *cls, PyObject * args)
{
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FCollectionNameType> collections;
	CollectionManager.GetCollections(collections);
	for (FCollectionNameType name_type : collections)
	{
		PyList_Append(py_list, Py_BuildValue((char *)"(si)", TCHAR_TO_UTF8(*name_type.Name.ToString()), (int)name_type.Type));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_get_parent_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	TOptional<FCollectionNameType> name_type = CollectionManager.GetParentCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type);
	if (name_type.IsSet())
	{
		return Py_BuildValue((char *)"(si)", TCHAR_TO_UTF8(*name_type.GetValue().Name.ToString()), (int)name_type.GetValue().Type);
	}
	Py_RETURN_NONE;
}

static PyObject *py_ue_icollection_manager_get_root_collections(PyObject *cls, PyObject * args)
{
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FCollectionNameType> collections;
	CollectionManager.GetRootCollections(collections);
	for (FCollectionNameType name_type : collections)
	{
		PyList_Append(py_list, Py_BuildValue((char *)"(si)", TCHAR_TO_UTF8(*name_type.Name.ToString()), (int)name_type.Type));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_get_child_collection_names(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	int child_type;

	if (!PyArg_ParseTuple(args, "sii", &name, &type, &child_type))
		return nullptr;
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> names;
	CollectionManager.GetChildCollectionNames(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, (ECollectionShareType::Type)child_type, names);
	for (FName cname : names)
	{
		PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*cname.ToString())));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_get_child_collections(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FCollectionNameType> collections;
	CollectionManager.GetChildCollections(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, collections);
	for (FCollectionNameType name_type : collections)
	{
		PyList_Append(py_list, Py_BuildValue((char *)"(si)", TCHAR_TO_UTF8(*name_type.Name.ToString()), (int)name_type.Type));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_get_collection_names(PyObject *cls, PyObject * args)
{
	int type;

	if (!PyArg_ParseTuple(args, "i", &type))
		return nullptr;
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> names;
	CollectionManager.GetCollectionNames((ECollectionShareType::Type)type, names);
	for (FName name : names)
	{
		PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString())));
	}
	return py_list;
}

static PyObject *py_ue_icollection_manager_get_root_collection_names(PyObject *cls, PyObject * args)
{
	int type;

	if (!PyArg_ParseTuple(args, "i", &type))
		return nullptr;
	PyObject *py_list = PyList_New(0);
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> names;
	CollectionManager.GetRootCollectionNames((ECollectionShareType::Type)type, names);
	for (FName name : names)
	{
		PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString())));
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

static PyObject *py_ue_icollection_manager_create_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	int storage;

	if (!PyArg_ParseTuple(args, "sii", &name, &type, &storage))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.CreateCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, (ECollectionStorageMode::Type)storage))
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

static PyObject *py_ue_icollection_manager_rename_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	char *new_name;
	int new_type;

	if (!PyArg_ParseTuple(args, "sisi", &name, &type, &new_name, &new_type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.RenameCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		FName(UTF8_TO_TCHAR(new_name)),
		(ECollectionShareType::Type)new_type
	))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_remove_from_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	char *object_path;

	if (!PyArg_ParseTuple(args, "sis", &name, &type, &object_path))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.RemoveFromCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		FName(UTF8_TO_TCHAR(object_path))
	))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_add_to_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	char *object_path;

	if (!PyArg_ParseTuple(args, "sis", &name, &type, &object_path))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.AddToCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		FName(UTF8_TO_TCHAR(object_path))
	))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_collection_exists(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.CollectionExists(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type
	))
		Py_RETURN_TRUE;
	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_create_unique_collection_name(PyObject *cls, PyObject * args)
{
	char *base_name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &base_name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	FName name;
	CollectionManager.CreateUniqueCollectionName(
		FName(UTF8_TO_TCHAR(base_name)),
		(ECollectionShareType::Type)type,
		name);

	return PyUnicode_FromString(TCHAR_TO_UTF8(*name.ToString()));
}

static PyObject *py_ue_icollection_manager_destroy_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.DestroyCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_has_collections(PyObject *cls, PyObject * args)
{
	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.HasCollections())
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_empty_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.EmptyCollection(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_get_dynamic_query_text(PyObject *cls, PyObject * args)
{
	char *name;
	int type;

	if (!PyArg_ParseTuple(args, "si", &name, &type))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	FString query_text;
	if (!CollectionManager.GetDynamicQueryText(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		query_text))
	{
		return PyErr_Format(PyExc_Exception, "unable to get dynamic query text");
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(*query_text));
}

static PyObject *py_ue_icollection_manager_set_dynamic_query_text(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	char *query_text;

	if (!PyArg_ParseTuple(args, "sis", &name, &type, &query_text))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	if (CollectionManager.SetDynamicQueryText(
		FName(UTF8_TO_TCHAR(name)),
		(ECollectionShareType::Type)type,
		FString(UTF8_TO_TCHAR(query_text))))
		Py_RETURN_TRUE;

	Py_RETURN_FALSE;
}

static PyObject *py_ue_icollection_manager_get_last_error(PyObject *cls, PyObject * args)
{

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();

	FText error = CollectionManager.GetLastError();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*error.ToString()));
}

static PyObject *py_ue_icollection_manager_get_assets_in_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	int recursion = 1;

	if (!PyArg_ParseTuple(args, "si|i", &name, &type, &recursion))
		return nullptr;

	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> paths;
	if (CollectionManager.GetAssetsInCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, paths, (ECollectionRecursionFlags::Flags)recursion))
	{
		PyObject *py_list = PyList_New(0);
		for (FName path : paths)
		{
			PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*path.ToString())));
		}
		return py_list;
	}
	return PyErr_Format(PyExc_Exception, "unable to retrieve the assets list from collection");
}

static PyObject *py_ue_icollection_manager_get_objects_in_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	int recursion = 1;

	if (!PyArg_ParseTuple(args, "si|i", &name, &type, &recursion))
		return nullptr;



	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> paths;
	if (CollectionManager.GetObjectsInCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, paths, (ECollectionRecursionFlags::Flags)recursion))
	{
		PyObject *py_list = PyList_New(0);
		for (FName path : paths)
		{
			PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*path.ToString())));
		}
		return py_list;
	}
	return PyErr_Format(PyExc_Exception, "unable to retrieve the objects list from collection");
}

static PyObject *py_ue_icollection_manager_get_classes_in_collection(PyObject *cls, PyObject * args)
{
	char *name;
	int type;
	int recursion = 1;

	if (!PyArg_ParseTuple(args, "si|i", &name, &type, &recursion))
		return nullptr;



	ICollectionManager &CollectionManager = FCollectionManagerModule::GetModule().Get();
	TArray<FName> paths;
	if (CollectionManager.GetClassesInCollection(FName(UTF8_TO_TCHAR(name)), (ECollectionShareType::Type)type, paths, (ECollectionRecursionFlags::Flags)recursion))
	{
		PyObject *py_list = PyList_New(0);
		for (FName path : paths)
		{
			PyList_Append(py_list, PyUnicode_FromString(TCHAR_TO_UTF8(*path.ToString())));
		}
		return py_list;
	}
	return PyErr_Format(PyExc_Exception, "unable to retrieve the classes list from collection");
}

static PyMethodDef ue_PyICollectionManager_methods[] = {
	{ "get_collections", (PyCFunction)py_ue_icollection_manager_get_collections, METH_VARARGS | METH_CLASS, "" },
	{ "get_root_collections", (PyCFunction)py_ue_icollection_manager_get_root_collections, METH_VARARGS | METH_CLASS, "" },
	{ "get_child_collections", (PyCFunction)py_ue_icollection_manager_get_child_collections, METH_VARARGS | METH_CLASS, "" },
	{ "get_root_collection_names", (PyCFunction)py_ue_icollection_manager_get_root_collection_names, METH_VARARGS | METH_CLASS, "" },
	{ "get_collection_names", (PyCFunction)py_ue_icollection_manager_get_collection_names, METH_VARARGS | METH_CLASS, "" },
	{ "create_static_collection", (PyCFunction)py_ue_icollection_manager_create_static_collection, METH_VARARGS | METH_CLASS, "" },
	{ "create_dynamic_collection", (PyCFunction)py_ue_icollection_manager_create_dynamic_collection, METH_VARARGS | METH_CLASS, "" },
	{ "create_collection", (PyCFunction)py_ue_icollection_manager_create_collection, METH_VARARGS | METH_CLASS, "" },
	{ "reparent_collection", (PyCFunction)py_ue_icollection_manager_reparent_collection, METH_VARARGS | METH_CLASS, "" },
	{ "rename_collection", (PyCFunction)py_ue_icollection_manager_rename_collection, METH_VARARGS | METH_CLASS, "" },
	{ "add_to_collection", (PyCFunction)py_ue_icollection_manager_add_to_collection, METH_VARARGS | METH_CLASS, "" },
	{ "collection_exists", (PyCFunction)py_ue_icollection_manager_collection_exists, METH_VARARGS | METH_CLASS, "" },
	{ "create_unique_collection_name", (PyCFunction)py_ue_icollection_manager_create_unique_collection_name, METH_VARARGS | METH_CLASS, "" },
	{ "destroy_collection", (PyCFunction)py_ue_icollection_manager_destroy_collection, METH_VARARGS | METH_CLASS, "" },
	{ "empty_collection", (PyCFunction)py_ue_icollection_manager_empty_collection, METH_VARARGS | METH_CLASS, "" },
	{ "get_dynamic_query_text", (PyCFunction)py_ue_icollection_manager_get_dynamic_query_text, METH_VARARGS | METH_CLASS, "" },
	{ "set_dynamic_query_text", (PyCFunction)py_ue_icollection_manager_set_dynamic_query_text, METH_VARARGS | METH_CLASS, "" },
	{ "remove_from_collection", (PyCFunction)py_ue_icollection_manager_remove_from_collection, METH_VARARGS | METH_CLASS, "" },
	{ "get_assets_in_collection", (PyCFunction)py_ue_icollection_manager_get_assets_in_collection, METH_VARARGS | METH_CLASS, "" },
	{ "get_objects_in_collection", (PyCFunction)py_ue_icollection_manager_get_objects_in_collection, METH_VARARGS | METH_CLASS, "" },
	{ "get_classes_in_collection", (PyCFunction)py_ue_icollection_manager_get_classes_in_collection, METH_VARARGS | METH_CLASS, "" },
	{ "get_parent_collection", (PyCFunction)py_ue_icollection_manager_get_parent_collection, METH_VARARGS | METH_CLASS, "" },
	{ "has_collections", (PyCFunction)py_ue_icollection_manager_has_collections, METH_VARARGS | METH_CLASS, "" },
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
