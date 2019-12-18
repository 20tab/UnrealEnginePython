#include "UEPyIConsoleManager.h"

static PyObject *py_ue_iconsole_manager_add_history_entry(PyObject *cls, PyObject * args)
{

	char *entry;
	if (!PyArg_ParseTuple(args, "s:add_history_entry", &entry))
	{
		return nullptr;
	}

#if ENGINE_MINOR_VERSION > 18
	IConsoleManager::Get().AddConsoleHistoryEntry(TEXT(""), UTF8_TO_TCHAR(entry));
#else
	IConsoleManager::Get().AddConsoleHistoryEntry(UTF8_TO_TCHAR(entry));
#endif

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_get_history(PyObject *cls, PyObject * args)
{
	TArray<FString> history;
#if ENGINE_MINOR_VERSION > 18
	IConsoleManager::Get().GetConsoleHistory(TEXT(""), history);
#else
	IConsoleManager::Get().GetConsoleHistory(history);
#endif
	PyObject *py_history = PyList_New(0);
	for (FString item : history)
	{
		PyObject *py_item = PyUnicode_FromString(TCHAR_TO_UTF8(*item));
		PyList_Append(py_history, py_item);
		Py_DECREF(py_item);
	}
	return py_history;
}

#if ENGINE_MINOR_VERSION > 12
static PyObject *py_ue_iconsole_manager_get_objects(PyObject *cls, PyObject * args)
{
	char *key = (char*)"";
	if (!PyArg_ParseTuple(args, "|s:get_objects", &key))
	{
		return nullptr;
	}

	PyObject *py_names = PyList_New(0);

	IConsoleManager::Get().ForEachConsoleObjectThatStartsWith(FConsoleObjectVisitor::CreateLambda([&](const TCHAR *name, IConsoleObject *object)
	{
		PyObject *py_item = PyUnicode_FromString(TCHAR_TO_UTF8(name));
		PyList_Append(py_names, py_item);
		Py_DECREF(py_item);
	}), UTF8_TO_TCHAR(key));

	return py_names;
}
#endif

#if ENGINE_MINOR_VERSION > 12
static PyObject *py_ue_iconsole_manager_get_objects_containing(PyObject *cls, PyObject * args)
{
	char *key = (char*)"";
	if (!PyArg_ParseTuple(args, "|s:get_objects_containing", &key))
	{
		return nullptr;
	}

	PyObject *py_names = PyList_New(0);

	IConsoleManager::Get().ForEachConsoleObjectThatContains(FConsoleObjectVisitor::CreateLambda([&](const TCHAR *name, IConsoleObject *object)
	{
		PyObject *py_item = PyUnicode_FromString(TCHAR_TO_UTF8(name));
		PyList_Append(py_names, py_item);
		Py_DECREF(py_item);
	}), UTF8_TO_TCHAR(key));

	return py_names;
}
#endif

static PyObject *py_ue_iconsole_manager_get_help(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:get_help", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	return PyUnicode_FromString(TCHAR_TO_UTF8(c_object->GetHelp()));
}

static PyObject *py_ue_iconsole_manager_set_help(PyObject *cls, PyObject * args)
{
	char *key;
	char *help;
	if (!PyArg_ParseTuple(args, "ss:set_help", &key, &help))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	c_object->SetHelp(UTF8_TO_TCHAR(help));

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_get_string(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:get_string", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	/*TConsoleVariableData<FString> *c_string = c_object->AsVariableString();
	if (!c_string)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a string", key);
	}*/

	return PyUnicode_FromString(TCHAR_TO_UTF8(*var->GetString()));
}

static PyObject *py_ue_iconsole_manager_get_int(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:get_int", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	TConsoleVariableData<int32> *c_int = c_object->AsVariableInt();
	if (!c_int)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not an int", key);
	}

	return PyLong_FromLong(var->GetInt());
}

static PyObject *py_ue_iconsole_manager_get_float(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:get_float", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	TConsoleVariableData<float> *c_float = c_object->AsVariableFloat();
	if (!c_float)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a float", key);
	}

	return PyFloat_FromDouble(var->GetFloat());
}

static PyObject *py_ue_iconsole_manager_set_string(PyObject *cls, PyObject * args)
{
	char *key;
	char *value;
	if (!PyArg_ParseTuple(args, "ss:set_string", &key, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	TConsoleVariableData<FString> *c_string = c_object->AsVariableString();
	if (!c_string)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a string", key);
	}

	var->Set(UTF8_TO_TCHAR(value));

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_set_int(PyObject *cls, PyObject * args)
{
	char *key;
	int value;
	if (!PyArg_ParseTuple(args, "si:set_int", &key, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	TConsoleVariableData<int32> *c_int = c_object->AsVariableInt();
	if (!c_int)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not an int", key);
	}

	var->Set(value);

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_set_float(PyObject *cls, PyObject * args)
{
	char *key;
	float value;
	if (!PyArg_ParseTuple(args, "sf:set_float", &key, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	IConsoleVariable *var = c_object->AsVariable();
	if (!var)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a variable", key);
	}

	TConsoleVariableData<float> *c_float = c_object->AsVariableFloat();
	if (!c_float)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" is not a float", key);
	}

	var->Set(value);

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_is_variable(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_variable", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	if (!c_object->AsVariable())
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_is_command(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_command", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	if (!c_object->AsCommand())
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_is_variable_string(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_variable_string", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	if (!c_object->AsVariableString())
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_is_variable_int(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_variable_int", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	if (!c_object->AsVariableInt())
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_is_variable_float(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_variable_float", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}

	if (!c_object->AsVariableFloat())
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_is_name_registered(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:is_name_registered", &key))
	{
		return nullptr;
	}

	if (!IConsoleManager::Get().IsNameRegistered(UTF8_TO_TCHAR(key)))
	{
		Py_RETURN_FALSE;
	}
	Py_RETURN_TRUE;
}

static PyObject *py_ue_iconsole_manager_unregister_object(PyObject *cls, PyObject * args)
{
	char *key;
	if (!PyArg_ParseTuple(args, "s:unregister_object", &key))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (!c_object)
	{
		return PyErr_Format(PyExc_Exception, "unable to find console object \"%s\"", key);
	}


	FPythonSmartConsoleDelegate::UnregisterPyDelegate(c_object);


	IConsoleManager::Get().UnregisterConsoleObject(c_object, false);

	Py_RETURN_NONE;

}

static PyObject *py_ue_iconsole_manager_register_variable_string(PyObject *cls, PyObject * args)
{
	char *key;
	char *help;
	char *value = (char *)"";
	if (!PyArg_ParseTuple(args, "ss|s:register_variable_string", &key, &help, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (c_object)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" already exists", key);
	}

	if (!IConsoleManager::Get().RegisterConsoleVariable(UTF8_TO_TCHAR(key), FString(UTF8_TO_TCHAR(value)), UTF8_TO_TCHAR(help), 0))
	{
		return PyErr_Format(PyExc_Exception, "unable to register console object \"%s\"", key);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_register_variable_int(PyObject *cls, PyObject * args)
{
	char *key;
	char *help;
	int value = 0;
	if (!PyArg_ParseTuple(args, "ss|i:register_variable_int", &key, &help, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (c_object)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" already exists", key);
	}

	if (!IConsoleManager::Get().RegisterConsoleVariable(UTF8_TO_TCHAR(key), value, UTF8_TO_TCHAR(help), 0))
	{
		return PyErr_Format(PyExc_Exception, "unable to register console object \"%s\"", key);
	}

	Py_RETURN_NONE;
}

static PyObject *py_ue_iconsole_manager_register_variable_float(PyObject *cls, PyObject * args)
{
	char *key;
	char *help;
	float value = 0;
	if (!PyArg_ParseTuple(args, "ss|f:register_variable_float", &key, &help, &value))
	{
		return nullptr;
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (c_object)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" already exists", key);
	}

	if (!IConsoleManager::Get().RegisterConsoleVariable(UTF8_TO_TCHAR(key), value, UTF8_TO_TCHAR(help), 0))
	{
		return PyErr_Format(PyExc_Exception, "unable to register console object \"%s\"", key);
	}

	Py_RETURN_NONE;
}

void FPythonSmartConsoleDelegate::OnConsoleCommand(const TArray < FString > & InArgs)
{
	FScopePythonGIL gil;

	PyObject *ret = nullptr;
	if (InArgs.Num() == 0)
	{
		ret = PyObject_CallFunction(py_callable, nullptr);
	}
	else
	{
		PyObject *py_args = PyTuple_New(InArgs.Num());
		for (int32 i = 0; i < InArgs.Num(); i++)
		{
			PyTuple_SetItem(py_args, i, PyUnicode_FromString(TCHAR_TO_UTF8(*InArgs[i])));
		}
		ret = PyObject_CallObject(py_callable, py_args);
		Py_DECREF(py_args);
	}
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

// static TArray declaration
TArray<FPythonSmartConsoleDelegatePair> FPythonSmartConsoleDelegate::PyDelegatesMapping;

static PyObject *py_ue_iconsole_manager_register_command(PyObject *cls, PyObject * args)
{
	char *key;
	PyObject *py_callable;
	char *help = nullptr;
	if (!PyArg_ParseTuple(args, "sO|s:register_command", &key, &py_callable, &help))
	{
		return nullptr;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "argument is not callable");
	}

	IConsoleObject *c_object = IConsoleManager::Get().FindConsoleObject(UTF8_TO_TCHAR(key));
	if (c_object)
	{
		return PyErr_Format(PyExc_Exception, "console object \"%s\" already exists", key);
	}

	TSharedRef<FPythonSmartConsoleDelegate> py_delegate = MakeShareable(new FPythonSmartConsoleDelegate);
	py_delegate->SetPyCallable(py_callable);
	FConsoleCommandWithArgsDelegate console_delegate;
	console_delegate.BindSP(py_delegate, &FPythonSmartConsoleDelegate::OnConsoleCommand);



	c_object = IConsoleManager::Get().RegisterConsoleCommand(UTF8_TO_TCHAR(key), help ? UTF8_TO_TCHAR(help) : UTF8_TO_TCHAR(key), console_delegate, 0);
	if (!c_object)
		return PyErr_Format(PyExc_Exception, "unable to register console command \"%s\"", key);

	// this allows the delegates to not be destroyed
	FPythonSmartConsoleDelegate::RegisterPyDelegate(c_object, py_delegate);

	Py_RETURN_NONE;
}


static PyMethodDef ue_PyIConsoleManager_methods[] = {
	{ "get_history", (PyCFunction)py_ue_iconsole_manager_get_history, METH_VARARGS | METH_CLASS, "" },
	{ "add_history_entry", (PyCFunction)py_ue_iconsole_manager_add_history_entry, METH_VARARGS | METH_CLASS, "" },
#if ENGINE_MINOR_VERSION > 12
	{ "get_objects", (PyCFunction)py_ue_iconsole_manager_get_objects, METH_VARARGS | METH_CLASS, "" },
	{ "get_objects_starting_with", (PyCFunction)py_ue_iconsole_manager_get_objects, METH_VARARGS | METH_CLASS, "" },
	{ "get_objects_containing", (PyCFunction)py_ue_iconsole_manager_get_objects_containing, METH_VARARGS | METH_CLASS, "" },
#endif
	{ "get_help", (PyCFunction)py_ue_iconsole_manager_get_help, METH_VARARGS | METH_CLASS, "" },
	{ "set_help", (PyCFunction)py_ue_iconsole_manager_set_help, METH_VARARGS | METH_CLASS, "" },
	{ "get_int", (PyCFunction)py_ue_iconsole_manager_get_int, METH_VARARGS | METH_CLASS, "" },
	{ "get_float", (PyCFunction)py_ue_iconsole_manager_get_float, METH_VARARGS | METH_CLASS, "" },
	{ "get_string", (PyCFunction)py_ue_iconsole_manager_get_string, METH_VARARGS | METH_CLASS, "" },
	{ "set_int", (PyCFunction)py_ue_iconsole_manager_set_int, METH_VARARGS | METH_CLASS, "" },
	{ "set_float", (PyCFunction)py_ue_iconsole_manager_set_float, METH_VARARGS | METH_CLASS, "" },
	{ "set_string", (PyCFunction)py_ue_iconsole_manager_set_string, METH_VARARGS | METH_CLASS, "" },
	{ "is_variable", (PyCFunction)py_ue_iconsole_manager_is_variable, METH_VARARGS | METH_CLASS, "" },
	{ "is_command", (PyCFunction)py_ue_iconsole_manager_is_command, METH_VARARGS | METH_CLASS, "" },
	{ "is_variable_string", (PyCFunction)py_ue_iconsole_manager_is_variable_string, METH_VARARGS | METH_CLASS, "" },
	{ "is_variable_float", (PyCFunction)py_ue_iconsole_manager_is_variable_float, METH_VARARGS | METH_CLASS, "" },
	{ "is_variable_int", (PyCFunction)py_ue_iconsole_manager_is_variable_int, METH_VARARGS | METH_CLASS, "" },
	{ "is_name_registered", (PyCFunction)py_ue_iconsole_manager_is_name_registered, METH_VARARGS | METH_CLASS, "" },
	{ "unregister_object", (PyCFunction)py_ue_iconsole_manager_unregister_object, METH_VARARGS | METH_CLASS, "" },
	{ "register_variable_string", (PyCFunction)py_ue_iconsole_manager_register_variable_string, METH_VARARGS | METH_CLASS, "" },
	{ "register_variable_int", (PyCFunction)py_ue_iconsole_manager_register_variable_int, METH_VARARGS | METH_CLASS, "" },
	{ "register_variable_float", (PyCFunction)py_ue_iconsole_manager_register_variable_float, METH_VARARGS | METH_CLASS, "" },
	{ "register_command", (PyCFunction)py_ue_iconsole_manager_register_command, METH_VARARGS | METH_CLASS, "" },
	{ NULL }  /* Sentinel */
};


static PyTypeObject ue_PyIConsoleManagerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.IConsoleManager", /* tp_name */
	sizeof(ue_PyIConsoleManager), /* tp_basicsize */
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
	"Unreal Engine ConsoleManager Interface",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PyIConsoleManager_methods,             /* tp_methods */
	0,
	0,
};

static int py_ue_iconsole_manager_init(ue_PyIConsoleManager *self, PyObject * args)
{
	PyErr_SetString(PyExc_Exception, "IConsoleManager is a singleton");
	return -1;
}

void ue_python_init_iconsole_manager(PyObject *ue_module)
{
	ue_PyIConsoleManagerType.tp_new = PyType_GenericNew;
	ue_PyIConsoleManagerType.tp_init = (initproc)py_ue_iconsole_manager_init;

	if (PyType_Ready(&ue_PyIConsoleManagerType) < 0)
		return;

	Py_INCREF(&ue_PyIConsoleManagerType);
	PyModule_AddObject(ue_module, "IConsoleManager", (PyObject *)&ue_PyIConsoleManagerType);
}
