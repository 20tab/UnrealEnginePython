#pragma once

#include "UnrealEnginePython.h"

#include "UEPySTreeView.h"

extern PyTypeObject ue_PySPythonTreeViewType;

class SPythonTreeView : public STreeView<TSharedPtr<struct FPythonItem>>
{
public:
	~SPythonTreeView()
	{
		delete(ItemsSource);
	}

	void SetPythonItemExpansion(PyObject *item, bool InShouldExpandItem);

    TSharedPtr<struct FPythonItem> const* SPythonTreeView::Find(PyObject *item);
};

typedef struct
{
	ue_PySTreeView s_tree_view;
	/* Type-specific fields go here. */
    TArray<TSharedPtr<struct FPythonItem>> item_source_list;
} ue_PySPythonTreeView;

void ue_python_init_spython_tree_view(PyObject *);
