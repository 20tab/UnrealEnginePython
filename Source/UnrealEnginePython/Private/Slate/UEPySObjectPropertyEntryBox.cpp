#include "UEPySObjectPropertyEntryBox.h"

#if WITH_EDITOR


static PyMethodDef ue_PySObjectPropertyEntryBox_methods[] = {
	{ NULL }  /* Sentinel */
};

DECLARE_UE_PY_SLATE_WIDGET(SObjectPropertyEntryBox);

static int ue_py_sobject_property_entry_box_init(ue_PySObjectPropertyEntryBox *self, PyObject *args, PyObject *kwargs)
{
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

	ue_py_snew(SObjectPropertyEntryBox);
	return 0;
}

void ue_python_init_sobject_property_entry_box(PyObject *ue_module)
{

	ue_PySObjectPropertyEntryBoxType.tp_init = (initproc)ue_py_sobject_property_entry_box_init;

	ue_PySObjectPropertyEntryBoxType.tp_base = &ue_PySCompoundWidgetType;

	if (PyType_Ready(&ue_PySObjectPropertyEntryBoxType) < 0)
		return;

	Py_INCREF(&ue_PySObjectPropertyEntryBoxType);
	PyModule_AddObject(ue_module, "SObjectPropertyEntryBox", (PyObject *)&ue_PySObjectPropertyEntryBoxType);
}
#endif
