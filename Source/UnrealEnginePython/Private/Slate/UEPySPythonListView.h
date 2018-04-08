#pragma once

#include "UEPySListView.h"

extern PyTypeObject ue_PySPythonListViewType;

class SPythonListView : public SListView<TSharedPtr<struct FPythonItem>>
{
public:
	~SPythonListView()
	{
		delete(ItemsSource);
	}

	void SetHeaderRow(TSharedPtr<SHeaderRow> InHeaderRowWidget);
};

typedef struct
{
	ue_PySListView s_list_view;
	/* Type-specific fields go here. */
	TArray<TSharedPtr<struct FPythonItem>> item_source_list;
} ue_PySPythonListView;

void ue_python_init_spython_list_view(PyObject *);
