#pragma once


#include "PythonSmartDelegate.h"

#include "UEPySlatePythonItem.h"
#include "Runtime/Slate/Public/Widgets/Views/SHeaderRow.h"
#include "Runtime/Slate/Public/Widgets/Docking/SDockTab.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxExtender.h"
#include "AssetData.h"

void unreal_engine_py_log_error();
template <typename T> T *ue_py_check_struct(PyObject *);


class FPythonSlateDelegate : public FPythonSmartDelegate
{

public:
	FReply OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event);
	FReply OnClicked();

	FReply OnKeyDown(const FGeometry &geometry, const FKeyEvent &key_event);
	void OnTextChanged(const FText &text);
	void OnTextCommitted(const FText &text, ETextCommit::Type commit_type);
	void OnInt32Changed(int32 value);
	void OnInt32Committed(int32 value, ETextCommit::Type commit_type);
	void OnFloatChanged(float value);
	void OnFloatCommitted(float value, ETextCommit::Type commit_type);
	void OnSort(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	void OnLinearColorChanged(FLinearColor color);

	void OnStringChanged(const FString &text);

	TSharedRef<SDockTab> SpawnPythonTab(const FSpawnTabArgs& args);

	TSharedRef<ITableRow> GenerateRow(TSharedPtr<FPythonItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void GetChildren(TSharedPtr<FPythonItem> InItem, TArray<TSharedPtr<FPythonItem>>& OutChildren);

#if WITH_EDITOR
	void OnAssetDoubleClicked(const FAssetData& AssetData);
	TSharedPtr<SWidget> OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets);
	void OnAssetSelected(const FAssetData& AssetData);
	TSharedRef<FExtender> OnExtendContentBrowserMenu(const TArray<FAssetData> &SelectedAssets);
	void MenuPyAssetBuilder(FMenuBuilder &Builder, TArray<FAssetData> SelectedAssets);
	void OnAssetChanged(const FAssetData &AssetData);
	bool OnShouldFilterAsset(const FAssetData& AssetData);
#endif

	void OnWindowClosed(const TSharedRef<SWindow> &Window);
	void SubMenuPyBuilder(FMenuBuilder &Builder);

	TSharedPtr<SWidget> OnContextMenuOpening();
	TSharedRef<SWidget> OnGenerateWidget(TSharedPtr<FPythonItem> py_item);
	TSharedRef<SWidget> OnGetMenuContent();
	void OnSelectionChanged(TSharedPtr<FPythonItem> py_item, ESelectInfo::Type select_type);

	void SimpleExecuteAction();
	void ExecuteAction(PyObject *py_obj);

	FText GetterFText() const;
	FString GetterFString() const;
	float GetterFloat() const;
	TOptional<float> GetterTFloat() const;
	int GetterInt() const;
	bool GetterBool() const;

	FVector2D GetterFVector2D() const;
	FLinearColor GetterFLinearColor() const;
	void CheckBoxChanged(ECheckBoxState state);


	template<typename T> T GetterIntT() const
	{
		FScopePythonGIL gil;

		PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return (T)0;
		}
		if (!PyNumber_Check(ret))
		{
			PyErr_SetString(PyExc_ValueError, "returned value is not a number");
			Py_DECREF(ret);
			return (T)0;
		}

		PyObject *py_int = PyNumber_Long(ret);
		int n = PyLong_AsLong(py_int);
		Py_DECREF(py_int);
		Py_DECREF(ret);
		return (T)n;
	}

	template<typename T> T GetterStructT() const
	{
		FScopePythonGIL gil;

		PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return T();
		}

		T *u_struct = ue_py_check_struct<T>(ret);
		if (!u_struct)
		{
			PyErr_SetString(PyExc_ValueError, "returned value is not a UStruct");
			Py_DECREF(ret);
			return T();
		}

		T u_struct_copy = *u_struct;
		Py_DECREF(ret);
		return u_struct_copy;
	}
};


