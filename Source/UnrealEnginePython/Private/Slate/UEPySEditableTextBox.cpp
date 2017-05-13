#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"

#include "UEPySEditableTextBox.h"



#define GET_s_editable_text_box TSharedRef<SEditableTextBox> s_editable_text_box = StaticCastSharedRef<SEditableTextBox>(self->s_border.s_compound_widget.s_widget.s_widget)

static PyObject *py_ue_seditable_text_box_select_all_text(ue_PySEditableTextBox *self, PyObject * args) {
	GET_s_editable_text_box;

	s_editable_text_box->SelectAllText();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_seditable_text_box_clear_selection(ue_PySEditableTextBox *self, PyObject * args) {
	GET_s_editable_text_box;

	s_editable_text_box->ClearSelection();

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject *py_ue_seditable_text_box_get_selected_text(ue_PySEditableTextBox *self, PyObject * args) {
	GET_s_editable_text_box;

	FText text = s_editable_text_box->GetSelectedText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *py_ue_seditable_text_box_get_text(ue_PySEditableTextBox *self, PyObject * args) {
	GET_s_editable_text_box;

	FText text = s_editable_text_box->GetText();

	return PyUnicode_FromString(TCHAR_TO_UTF8(*text.ToString()));
}

static PyObject *ue_PySEditableTextBox_str(ue_PySEditableTextBox *self)
{
	return PyUnicode_FromFormat("<unreal_engine.SEditableTextBox '%p'>",
		&self->s_border.s_compound_widget.s_widget.s_widget.Get());
}

static PyMethodDef ue_PySEditableTextBox_methods[] = {
	{ "select_all_text", (PyCFunction)py_ue_seditable_text_box_select_all_text, METH_VARARGS, "" },
	{ "clear_selection", (PyCFunction)py_ue_seditable_text_box_clear_selection, METH_VARARGS, "" },
	{ "get_selected_text", (PyCFunction)py_ue_seditable_text_box_get_selected_text, METH_VARARGS, "" },
	{ "get_text", (PyCFunction)py_ue_seditable_text_box_get_text, METH_VARARGS, "" },
	{ NULL }  /* Sentinel */
};

PyTypeObject ue_PySEditableTextBoxType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	"unreal_engine.SEditableTextBox", /* tp_name */
	sizeof(ue_PySEditableTextBox), /* tp_basicsize */
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
	(reprfunc)ue_PySEditableTextBox_str,                         /* tp_str */
	0,                         /* tp_getattro */
	0,                         /* tp_setattro */
	0,                         /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /* tp_flags */
	"Unreal Engine SEditableTextBox",           /* tp_doc */
	0,                         /* tp_traverse */
	0,                         /* tp_clear */
	0,                         /* tp_richcompare */
	0,                         /* tp_weaklistoffset */
	0,                         /* tp_iter */
	0,                         /* tp_iternext */
	ue_PySEditableTextBox_methods,             /* tp_methods */
};

static int ue_py_seditable_text_box_init(ue_PySEditableTextBox *self, PyObject *args, PyObject *kwargs) {
	self->s_border.s_compound_widget.s_widget.s_widget = TSharedRef<SEditableTextBox>(SNew(SEditableTextBox));
	return 0;
}

void ue_python_init_seditable_text_box(PyObject *ue_module) {
	ue_PySEditableTextBoxType.tp_new = PyType_GenericNew;

	ue_PySEditableTextBoxType.tp_init = (initproc)ue_py_seditable_text_box_init;

	ue_PySEditableTextBoxType.tp_base = &ue_PySBorderType;

	if (PyType_Ready(&ue_PySEditableTextBoxType) < 0)
		return;

	Py_INCREF(&ue_PySEditableTextBoxType);
	PyModule_AddObject(ue_module, "SEditableTextBox", (PyObject *)&ue_PySEditableTextBoxType);
}


#endif