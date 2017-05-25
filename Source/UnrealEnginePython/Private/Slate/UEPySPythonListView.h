#pragma once

#include "UnrealEnginePython.h"

#include "UEPySListView.h"

extern PyTypeObject ue_PySPythonListViewType;

class SPythonListView : public SListView<TSharedPtr<PyObject>> {
public:
	void SetPyCallable(PyObject *py_obj) {
		Py_INCREF(py_obj);
		py_callable = py_obj;
	}
protected:
	PyObject *py_items;
	PyObject *py_callable;
};

typedef struct {
	ue_PySListView s_list_view;
	/* Type-specific fields go here. */
} ue_PySPythonListView;

void ue_python_init_spython_list_view(PyObject *);
