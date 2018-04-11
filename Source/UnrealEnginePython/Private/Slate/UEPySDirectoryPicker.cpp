#include "UEPySDirectoryPicker.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 13


static PyObject *py_ue_sdirectory_picker_get_directory(ue_PySDirectoryPicker *self, PyObject * args)
{
	ue_py_slate_cast(SDirectoryPicker);
	FString Directory = py_SDirectoryPicker->GetDirectory();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*Directory));
}

static PyObject *py_ue_sdirectory_picker_get_file_path(ue_PySDirectoryPicker *self, PyObject * args)
{
	ue_py_slate_cast(SDirectoryPicker);
	FString FilePath = py_SDirectoryPicker->GetFilePath();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*FilePath));
}


static PyMethodDef ue_PySDirectoryPicker_methods[] = {
	{ "get_directory", (PyCFunction)py_ue_sdirectory_picker_get_directory, METH_VARARGS, "" },
	{ "get_file_path", (PyCFunction)py_ue_sdirectory_picker_get_file_path, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(SDirectoryPicker);

DECLARE_DELEGATE_OneParam(FOnDirectoryChanged, const FString& /*Directory*/);

static int ue_py_sdirectory_picker_init(ue_PySDirectoryPicker *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SDirectoryPicker);

	ue_py_slate_farguments_optional_text("message", Message);
	ue_py_slate_farguments_optional_string("directory", Directory);
	ue_py_slate_farguments_optional_string("file", File);
	ue_py_slate_farguments_bool("is_enabled", IsEnabled);
	ue_py_slate_farguments_event("on_directory_changed", OnDirectoryChanged, FOnDirectoryChanged, OnStringChanged);

	ue_py_snew(SDirectoryPicker);
	return 0;
}

void ue_python_init_sdirectory_picker(PyObject *ue_module)
{

	ue_PySDirectoryPickerType.tp_init = (initproc)ue_py_sdirectory_picker_init;

	ue_PySDirectoryPickerType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySDirectoryPickerType) < 0)
		return;

	Py_INCREF(&ue_PySDirectoryPickerType);
	PyModule_AddObject(ue_module, "SDirectoryPicker", (PyObject *)&ue_PySDirectoryPickerType);
}
#endif
#endif
