#pragma once

#include "UnrealEnginePython.h"

#include "UEPySPanel.h"

#include "Runtime/Slate/Public/Widgets/Input/SComboBox.h"

extern PyTypeObject ue_PySPythonComboBoxType;

class SPythonComboBox : public SComboBox<TSharedPtr<FPythonItem>> 
{

};

typedef struct {
	ue_PySPanel s_panel;
	/* Type-specific fields go here. */
    TArray<TSharedPtr<struct FPythonItem>> options_source_list;
} ue_PySPythonComboBox;

void ue_python_init_spython_combo_box(PyObject *);
