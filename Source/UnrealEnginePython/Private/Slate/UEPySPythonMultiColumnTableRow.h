#pragma once

#include "UEPySCompoundWidget.h"
#include "UnrealEnginePython.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"
#include "Runtime/Slate/Public/Widgets/Views/STableViewBase.h"
#include "Runtime/Slate/Public/Widgets/Views/SListView.h"

extern PyTypeObject ue_PySCompoundWidgetType;
extern PyTypeObject ue_PySTableViewBaseType;
class SPythonMultiColumnTableRow : public SMultiColumnTableRow<TSharedPtr<FPythonItem>>
{
public:
	SLATE_BEGIN_ARGS(SPythonMultiColumnTableRow) {}
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, PyObject *in_py_self)
	{
		SetPyObject(in_py_self);
		SMultiColumnTableRow<TSharedPtr<FPythonItem>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
	}

	TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName)
	{
		FScopePythonGIL gil;

		if (!PyObject_HasAttrString(self, (char *)"generate_widget_for_column"))
			return SNullWidget::NullWidget;

		PyObject *py_callable_generate_widget_for_column = PyObject_GetAttrString(self, (char *)"generate_widget_for_column");
		if (!PyCallable_Check(py_callable_generate_widget_for_column))
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

		Py_DECREF(ret);
		return Widget.ToSharedRef();
	}


	FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
	{
		FScopePythonGIL gil;

		if (PyObject_HasAttrString(self, (char *)"on_mouse_button_double_click"))
		{
			PyObject *py_callable_on_mouse_button_double_click = PyObject_GetAttrString(self, (char *)"on_mouse_button_double_click");
			if (!PyCallable_Check(py_callable_on_mouse_button_double_click))
			{
				UE_LOG(LogPython, Error, TEXT("on_mouse_button_double_click is not a callable"));
				return FReply::Unhandled();
			}

			PyObject *ret = PyObject_CallFunction(py_callable_on_mouse_button_double_click, (char *)"NN", py_ue_new_fgeometry(InMyGeometry), py_ue_new_fpointer_event(InMouseEvent));
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
			return SPythonMultiColumnTableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
		}
	}

	void SetPyObject(PyObject *py_obj)
	{
		self = py_obj;
	}
private:
	PyObject * self = nullptr;

	TSharedPtr<FPythonItem> RowPythonObject;
};

typedef struct
{
	ue_PySCompoundWidget s_compound_widget;
	/* Type-specific fields go here. */
} ue_PySPythonMultiColumnTableRow;

void ue_python_init_spython_multicolumn_table_row(PyObject *);

ue_PySPythonMultiColumnTableRow *py_ue_is_spython_multicolumn_table_row(PyObject *);
