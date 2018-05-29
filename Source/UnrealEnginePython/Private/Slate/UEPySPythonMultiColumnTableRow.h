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
class SPythonMultiColumnTableRow : public SMultiColumnTableRow<TSharedPtr<struct FPythonItem>>
{
public:
    SLATE_BEGIN_ARGS(SPythonMultiColumnTableRow) { }
    SLATE_END_ARGS();

    void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, PyObject *in_py_self)
    {
        SetPyObject(in_py_self);
        FSuperRowType::Construct(FSuperRowType::FArguments(), InOwnerTableView);
    }

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        FScopePythonGIL gil;

        if (!PyObject_HasAttrString(self, (char *)"generate_widget_for_column"))
            return SNullWidget::NullWidget;

        PyObject *py_callable_generate_widget_for_column = PyObject_GetAttrString(self, (char *)"generate_widget_for_column");
        if (!PyCalllable_Check_Extended(py_callable_generate_widget_for_column))
        {
            UE_LOG(LogPython, Error, TEXT("generate_widget_for_column is not a callable"));
            return SNullWidget::NullWidget;
        }

        PyObject *ret = PyObject_CallFunction(py_callable_generate_widget_for_column, (char *)"s", TCHAR_TO_UTF8(*ColumnName.ToString()));
        if (!ret)
        {
            unreal_engine_py_log_error();
            return SNullWidget::NullWidget;
        }

		TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(ret);
		if (!Widget.IsValid())
        {
            Py_DECREF(ret);
            UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
            return SNullWidget::NullWidget;
        }

        if (ColumnName == firstColumnName && OwnerTablePtr.Pin()->GetTableViewMode() == ETableViewMode::Tree)
        {
            // Rows in a tree need to show an SExpanderArrow (it also indents!) to give the appearance of being a tree.
            TSharedRef<SWidget> value = SNew(SHorizontalBox)
                +SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Top)
                [
                    SNew(SExpanderArrow, SharedThis(this))
                ]
                +SHorizontalBox::Slot()
                .AutoWidth()
                [
                    Widget.ToSharedRef()
                ];
            Py_DECREF(ret);
            return value;
        }
        else
        {
            // Lists do not need an expander arrow
            Py_DECREF(ret);
		    return Widget.ToSharedRef();
        }
    }


    FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
    {
        FScopePythonGIL gil;

        if (PyObject_HasAttrString(self, (char *)"on_mouse_button_double_click"))
        {
            PyObject *py_callable_on_mouse_button_double_click = PyObject_GetAttrString(self, (char *)"on_mouse_button_double_click");
            if (!PyCalllable_Check_Extended(py_callable_on_mouse_button_double_click))
            {
                UE_LOG(LogPython, Error, TEXT("on_mouse_button_double_click is not a callable"));
                return FReply::Unhandled();
            }

            PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_button_double_click, (char *)"OO", py_ue_new_fgeometry(InMyGeometry), py_ue_new_fpointer_event(InMouseEvent));
            if (!ret)
            {
                unreal_engine_py_log_error();
                return FReply::Unhandled();
            }

            if (ret == Py_False)
            {
                Py_DECREF(ret);
                return FReply::Unhandled();
            }
            Py_DECREF(ret);
            return FReply::Handled();
        }
        else
        {
            return FSuperRowType::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
        }
    }

    void SetPyObject(PyObject *py_obj)
    {
        self = py_obj;
    }

    void SetFirstColumnName(FName InFirstColName)
    {
        firstColumnName = InFirstColName;
    }
private:
    PyObject *self = nullptr;

    TSharedPtr<struct FPythonItem> RowPythonObject;

    FName firstColumnName = FName("friendlyName");
};

typedef struct
{
    ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
    ue_PySTableViewBase* owner_table;
} ue_PySPythonMultiColumnTableRow;

void ue_python_init_spython_multicolumn_table_row(PyObject *);

ue_PySPythonMultiColumnTableRow *py_ue_is_spython_multicolumn_table_row(PyObject *);
