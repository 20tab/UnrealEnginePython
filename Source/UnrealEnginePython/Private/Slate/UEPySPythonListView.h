#pragma once

#include "UnrealEnginePython.h"

#include "UEPySListView.h"

extern PyTypeObject ue_PySPythonListViewType;

class SPythonListView : public SListView<TSharedPtr<struct FPythonItem>> {
public:
	~SPythonListView() {
		delete(ItemsSource);
	}
};

typedef struct {
	ue_PySListView s_list_view;
	/* Type-specific fields go here. */
} ue_PySPythonListView;

void ue_python_init_spython_list_view(PyObject *);
