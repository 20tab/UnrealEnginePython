#include "UEPySFilePathPicker.h"

#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 13

static PyMethodDef ue_PySFilePathPicker_methods[] = {
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(SFilePathPicker);

static int ue_py_sfile_path_picker_init(ue_PySFilePathPicker *self, PyObject *args, PyObject *kwargs)
{
	ue_py_slate_setup_farguments(SFilePathPicker);

	ue_py_slate_farguments_optional_struct_ptr("browse_button_image", BrowseButtonImage, FSlateBrush);
	ue_py_slate_farguments_optional_struct_ptr("browse_button_style", BrowseButtonStyle, FButtonStyle);
	ue_py_slate_farguments_text("browse_button_tool_tip", BrowseButtonToolTip);
	ue_py_slate_farguments_string("browse_directory", BrowseDirectory);
	ue_py_slate_farguments_text("browse_title", BrowseTitle);
	ue_py_slate_farguments_string("file_path", FilePath);
	ue_py_slate_farguments_string("file_type_filter", FileTypeFilter);
	ue_py_slate_farguments_struct("font", Font, FSlateFontInfo);
	ue_py_slate_farguments_bool("is_read_only", IsReadOnly);
	ue_py_slate_farguments_event("on_path_picked", OnPathPicked, FOnPathPicked, OnStringChanged);

	ue_py_snew(SFilePathPicker);
	return 0;
}

void ue_python_init_sfile_path_picker(PyObject *ue_module)
{

	ue_PySFilePathPickerType.tp_init = (initproc)ue_py_sfile_path_picker_init;

	ue_PySFilePathPickerType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySFilePathPickerType) < 0)
		return;

	Py_INCREF(&ue_PySFilePathPickerType);
	PyModule_AddObject(ue_module, "SFilePathPicker", (PyObject *)&ue_PySFilePathPickerType);
}
#endif
#endif
