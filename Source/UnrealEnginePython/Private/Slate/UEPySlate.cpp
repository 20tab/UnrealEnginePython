#if WITH_EDITOR

#include "UnrealEnginePythonPrivatePCH.h"



#include "UEPySlate.h"

FReply UPythonSlateDelegate::OnMouseButtonDown(const FGeometry &geometry, const FPointerEvent &pointer_event) {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"OO", py_ue_new_uscriptstruct(FGeometry::StaticStruct(), (uint8 *)&geometry), py_ue_new_uscriptstruct(FPointerEvent::StaticStruct(), (uint8 *)&pointer_event));
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}

FReply UPythonSlateDelegate::OnClicked() {
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret) {
		unreal_engine_py_log_error();
		return FReply::Unhandled();
	}

	if (ret == Py_False) {
		Py_DECREF(ret);
		return FReply::Unhandled();
	}
	Py_DECREF(ret);
	return FReply::Handled();
}


void ue_python_init_slate(PyObject *module) {
	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
	ue_python_init_sborder(module);
	ue_python_init_sbutton(module);
	ue_python_init_sbutton(module);
	ue_python_init_sleaf_widget(module);
	ue_python_init_stext_block(module);
	ue_python_init_seditable_text_box(module);
}


#endif