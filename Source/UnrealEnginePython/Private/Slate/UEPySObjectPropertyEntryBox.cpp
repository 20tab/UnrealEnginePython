#if WITH_EDITOR
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySObjectPropertyEntryBox.h"

#define sw_object_property_entry_box StaticCastSharedRef<SObjectPropertyEntryBox>(self->s_compound_widget.s_widget.s_widget)


static PyMethodDef ue_PySObjectPropertyEntryBox_methods[] = {
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySObjectPropertyEntryBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SObjectPropertyEntryBox", /* tp_name */
	sizeof(ue_PySObjectPropertyEntryBox), /* tp_basicsize */
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
	"Unreal Engine SObjectPropertyEntryBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySObjectPropertyEntryBox_methods,             /* tp_methods */
};

static int ue_py_sobject_property_entry_box_init(ue_PySObjectPropertyEntryBox *self, PyObject *args, PyObject *kwargs) {
	ue_py_slate_setup_farguments(SObjectPropertyEntryBox);

	ue_py_slate_farguments_optional_bool("allow_clear", AllowClear);
	ue_py_slate_farguments_optional_bool("display_browse", DisplayBrowse);
	ue_py_slate_farguments_optional_bool("display_use_selected", DisplayUseSelected);
#if ENGINE_MINOR_VERSION > 13
	ue_py_slate_farguments_optional_bool("enable_content_picker", EnableContentPicker);
#endif
	ue_py_slate_farguments_string("object_path", ObjectPath);

	ue_py_slate_farguments_optional_uobject("allowed_class", AllowedClass, UClass);

	ue_py_slate_farguments_event("on_object_changed", OnObjectChanged, FOnSetObject, OnAssetChanged);
	ue_py_slate_farguments_event("on_should_filter_asset", OnShouldFilterAsset, FOnShouldFilterAsset, OnShouldFilterAsset);

	ue_py_snew(SObjectPropertyEntryBox, s_compound_widget.s_widget);
	return 0;
}

void ue_python_init_sobject_property_entry_box(PyObject *ue_module) {

	ue_PySObjectPropertyEntryBoxType.tp_init = (initproc)ue_py_sobject_property_entry_box_init;

	ue_PySObjectPropertyEntryBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySObjectPropertyEntryBoxType) < 0)
		return;

	Py_INCREF(&ue_PySObjectPropertyEntryBoxType);
	PyModule_AddObject(ue_module, "SObjectPropertyEntryBox", (PyObject *)&ue_PySObjectPropertyEntryBoxType);
}
#endif
