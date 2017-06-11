#if WITH_EDITOR
#if ENGINE_MINOR_VERSION > 13
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySFilePathPicker.h"

#define sw_file_path_picker StaticCastSharedRef<SFilePathPicker>(self->s_compound_widget.s_widget.s_widget)


static PyMethodDef ue_PySFilePathPicker_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySFilePathPickerType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SFilePathPicker", /* tp_name */
	sizeof(ue_PySFilePathPicker), /* tp_basicsize */
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
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SFilePathPicker",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySFilePathPicker_methods,             /* tp_methods */
};

static int ue_py_sfile_path_picker_init(ue_PySFilePathPicker *self, PyObject *args, PyObject *kwargs) {
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

	ue_py_snew(SFilePathPicker, s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sfile_path_picker(PyObject *ue_module) {

	ue_PySFilePathPickerType.tp_init = (initproc)ue_py_sfile_path_picker_init;

	ue_PySFilePathPickerType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySFilePathPickerType) < 0)
		return;

	Py_INCREF(&ue_PySFilePathPickerType);
	PyModule_AddObject(ue_module, "SFilePathPicker", (PyObject *)&ue_PySFilePathPickerType);
}
#endif
#endif
