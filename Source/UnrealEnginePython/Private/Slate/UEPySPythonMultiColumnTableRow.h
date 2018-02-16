#pragma once

#include "UnrealEnginePython.h"

#include "UEPySCompoundWidget.h"
#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"
#include "UEPyFCharacterEvent.h"
#include "UEPyFKeyEvent.h"
#include "UEPyFPointerEvent.h"

extern PyTypeObject ue_PySCompoundWidgetType;
extern PyTypeObject ue_PySTableViewBaseType;
class SPythonMultiColumnTableRow : public SMultiColumnTableRow<TSharedPtr<struct FPythonItem>> {
public:
    SLATE_BEGIN_ARGS(SPythonMultiColumnTableRow) { }
    SLATE_END_ARGS();

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, PyObject *in_py_self)
    {
        SetPyObject(in_py_self);
        SMultiColumnTableRow<TSharedPtr<struct FPythonItem> >::Construct(FSuperRowType::FArguments(), InOwnerTableView);
    }

    TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName);
    FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);

    void SetPyObject(PyObject *py_obj)
    {
        self = py_obj;
    }
private:
    PyObject *self = nullptr;

    TSharedPtr<struct FPythonItem> RowPythonObject;
};

typedef struct {
    ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySPythonMultiColumnTableRow;

void ue_python_init_spython_multicolumn_table_row(PyObject *);

ue_PySPythonMultiColumnTableRow *py_ue_is_spython_multicolumn_table_row(PyObject *);