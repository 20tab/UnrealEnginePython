
#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySPythonComboBox.h"


#define sw_python_combo_box StaticCastSharedRef<SPythonComboBox>(self->s_panel.s_widget.s_widget)

static PyObject *py_ue_spython_combo_box_clear_selection(ue_PySPythonComboBox *self, PyObject * args) {
	sw_python_combo_box->ClearSelection();
	Py_INCREF(self);
	return (PyObject *)self;
}

static PyObject *py_ue_spython_combo_box_get_selected_item(ue_PySPythonComboBox *self, PyObject * args) {

	TSharedPtr<FPythonItem> ptr_item = sw_python_combo_box->GetSelectedItem();
	if (!ptr_item.IsValid())
		return PyErr_Format(PyExc_Exception, "invalid shared pointer to python item");

	Py_INCREF(ptr_item.Get()->py_object);
	return ptr_item.Get()->py_object;
}

static PyObject *py_ue_spython_combo_box_set_selected_item(ue_PySPythonComboBox *self, PyObject * args) {
	PyObject *py_item;
	if (!PyArg_ParseTuple(args, "O", &py_item)) {
		return nullptr;
	}

	for (TSharedPtr<FPythonItem> item : *(sw_python_combo_box->PythonOptionsSource)) {
		// just for being safe
		if (!item.IsValid())
			continue;
		if (py_item == item.Get()->py_object) {
			sw_python_combo_box->SetSelectedItem(item);
			break;
		}
	}

	Py_INCREF(self);
	return (PyObject *)self;
}

static PyMethodDef ue_PySPythonComboBox_methods[] = {
	{ "clear_selection", (PyCFunction)py_ue_spython_combo_box_clear_selection, METH_VARARGS, "" },
	{ "get_selected_item", (PyCFunction)py_ue_spython_combo_box_get_selected_item, METH_VARARGS, "" },
	{ "set_selected_item", (PyCFunction)py_ue_spython_combo_box_set_selected_item, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySPythonComboBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SPythonComboBox", /* tp_name */
	sizeof(ue_PySPythonComboBox), /* tp_basicsize */
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
	"Unreal Engine SPythonComboBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySPythonComboBox_methods,             /* tp_methods */
};

static int ue_py_spython_combo_box_init(ue_PySPythonComboBox *self, PyObject *args, PyObject *kwargs) {
	ue_py_slate_setup_farguments(SPythonComboBox);

	// first of all check for values
	PyObject *values = ue_py_dict_get_item(kwargs, "options_source");
	if (!values) {
		PyErr_SetString(PyExc_Exception, "you must specify the combo box items");
		return -1;
	}

	values = PyObject_GetIter(values);
	if (!values) {
		PyErr_SetString(PyExc_Exception, "values field is not an iterable");
		return -1;
	}

	TArray<TSharedPtr<FPythonItem>> *items = new TArray<TSharedPtr<FPythonItem>>();
	while (PyObject *item = PyIter_Next(values)) {
		Py_INCREF(item);
		items->Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}
	Py_DECREF(values);

	arguments.OptionsSource(items);

	ue_PySWidget *s_widget_content = nullptr;

	PyObject *content = ue_py_dict_get_item(kwargs, "content");
	if (content) {
		s_widget_content = py_ue_is_swidget(content);
		if (!s_widget_content) {
			PyErr_SetString(PyExc_Exception, "content is not a SWidget");
			return -1;
		}
		Py_INCREF(s_widget_content);
		arguments.Content()[s_widget_content->s_widget];
	}

	ue_py_slate_farguments_optional_struct_ptr("button_style", ButtonStyle, FButtonStyle);
	ue_py_slate_farguments_struct("content_padding", ContentPadding, FMargin);
#if ENGINE_MINOR_VERSION > 13
	ue_py_slate_farguments_optional_bool("enable_gamepad_navigation_mode", EnableGamepadNavigationMode);
#endif
	ue_py_slate_farguments_struct("foreground_color", ForegroundColor, FSlateColor);
	ue_py_slate_farguments_optional_bool("has_down_arrow", HasDownArrow);
#if ENGINE_MINOR_VERSION > 12
	ue_py_slate_farguments_optional_struct_ptr("item_style", ItemStyle, FTableRowStyle);
#endif
	ue_py_slate_farguments_optional_float("max_list_height", MaxListHeight);
	ue_py_slate_farguments_optional_enum("method", Method, EPopupMethod);
	ue_py_slate_farguments_optional_struct("pressed_sound_override", PressedSoundOverride, FSlateSound);
	ue_py_slate_farguments_optional_struct("selection_change_sound_override", SelectionChangeSoundOverride, FSlateSound);
	ue_py_slate_farguments_event("on_generate_widget", OnGenerateWidget, TSlateDelegates<TSharedPtr<FPythonItem>>::FOnGenerateWidget, OnGenerateWidget);
	ue_py_slate_farguments_event("on_selection_changed", OnSelectionChanged, TSlateDelegates<TSharedPtr<FPythonItem>>::FOnSelectionChanged, OnSelectionChanged);


	ue_py_snew(SPythonComboBox, s_panel.s_widget);

	// keep track of the list, so we can delete on destruction
	sw_python_combo_box->PythonOptionsSource = items;

	return 0;
}

void ue_python_init_spython_combo_box(PyObject *ue_module) {

	ue_PySPythonComboBoxType.tp_base = &ue_PySPanelType;
	ue_PySPythonComboBoxType.tp_init = (initproc)ue_py_spython_combo_box_init;

	if (PyType_Ready(&ue_PySPythonComboBoxType) < 0)
		return;

	Py_INCREF(&ue_PySPythonComboBoxType);
	PyModule_AddObject(ue_module, "SPythonComboBox", (PyObject *)&ue_PySPythonComboBoxType);
}
