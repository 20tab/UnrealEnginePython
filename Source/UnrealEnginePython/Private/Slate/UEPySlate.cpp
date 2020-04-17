
#include "UEPySlate.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "Editor/UnrealEd/Public/Toolkits/AssetEditorToolkit.h"
#include "Editor/Persona/Public/PersonaModule.h"
#if ENGINE_MINOR_VERSION > 13
#include "Editor/AnimationEditor/Public/IAnimationEditorModule.h"
#endif
#include "Editor/StaticMeshEditor/Public/StaticMeshEditorModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/ISinglePropertyView.h"
#include "Editor/PropertyEditor/Public/IDetailsView.h"
#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
#endif

#include "Runtime/Slate/Public/Framework/Commands/UICommandList.h"
#include "Runtime/Slate/Public/Framework/Commands/UICommandInfo.h"
#include "Runtime/Slate/Public/Framework/Docking/TabManager.h"
#include "Runtime/Slate/Public/Widgets/Views/STableRow.h"
#include "Runtime/SlateCore/Public/Styling/SlateIconFinder.h"

#include "Runtime/AppFramework/Public/Widgets/Colors/SColorPicker.h"

#include "UEPySlate.h"
#include "PyNativeWidgetHost.h"

#include "Wrappers/UEPyFAssetData.h"

#include "Sound/SoundBase.h"

#include "UEPySWidget.h"
#include "UEPySCompoundWidget.h"
#include "UEPySWindow.h"
#include "UEPySBorder.h"
#include "UEPySButton.h"
#include "UEPySLeafWidget.h"
#include "UEPySTextBlock.h"
#include "UEPySEditableTextBox.h"
#include "UEPySMultiLineEditableText.h"
#include "UEPySPanel.h"
#include "UEPySGridPanel.h"
#include "UEPySBoxPanel.h"
#include "UEPySHorizontalBox.h"
#include "UEPySVerticalBox.h"
#include "UEPySViewport.h"


#include "UEPySImage.h"
#include "UEPySDockTab.h"
#include "UEPySTableViewBase.h"
#include "UEPySListView.h"
#include "UEPySPythonListView.h"
#include "UEPySPythonMultiColumnTableRow.h"
#include "UEPySTreeView.h"
#include "UEPySPythonTreeView.h"
#include "UEPySSplitter.h"
#include "UEPySHeaderRow.h"
#include "UEPySCheckBox.h"
#include "UEPySNumericEntryBox.h"
#include "UEPySCanvas.h"
#include "UEPySSlider.h"
#include "UEPySVectorInputBox.h"
#include "UEPySRotatorInputBox.h"
#include "UEPySPythonComboBox.h"
#include "UEPySScrollBox.h"
#include "UEPySColorBlock.h"
#include "UEPySBox.h"
#include "UEPySProgressBar.h"
#include "UEPySSpacer.h"
#include "UEPySPythonWidget.h"
#include "UEPySOverlay.h"

#include "UEPyFTabManager.h"
#include "UEPyFTabSpawnerEntry.h"
#include "UEPyFMenuBuilder.h"
#include "UEPyFToolBarBuilder.h"
#include "UEPyFSlateIcon.h"
#include "UEPyFSlateStyleSet.h"

#include "UEPyFGeometry.h"
#include "UEPyFPaintContext.h"

#include "UEPyFInputEvent.h"
#include "UEPyFPointerEvent.h"
#include "UEPyFKeyEvent.h"
#include "UEPyFCharacterEvent.h"
#include "UEPyFModifierKeysState.h"
#include "Wrappers/UEPyESlateEnums.h"

#if WITH_EDITOR
#include "UEPySEditorViewport.h"
#include "UEPySLevelViewport.h"
#include "UEPySPythonEditorViewport.h"
#include "UEPySGraphEditor.h"
#include "UEPySPythonShelf.h"
#include "UEPySFilePathPicker.h"
#include "UEPySDirectoryPicker.h"
#include "UEPySDropTarget.h"
#include "UEPySAssetDropTarget.h"
#include "UEPySObjectPropertyEntryBox.h"
#include "UEPyIDetailsView.h"
#include "UEPyIStructureDetailsView.h"
#include "UEPySNodePanel.h"
#include "UEPySGraphPanel.h"
#endif

#include "Runtime/Core/Public/Misc/Attribute.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"
#include "Runtime/SlateCore/Public/Styling/SlateStyleRegistry.h"
#include "Runtime/Slate/Public/Framework/Commands/Commands.h"

FReply FPythonSlateDelegate::OnMouseEvent(const FGeometry &geometry, const FPointerEvent &pointer_event)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"NN", py_ue_new_fgeometry(geometry), py_ue_new_fpointer_event(pointer_event));
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

FReply FPythonSlateDelegate::OnKeyDown(const FGeometry &geometry, const FKeyEvent &key_event)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"NN", py_ue_new_fgeometry(geometry), py_ue_new_fkey_event(key_event));
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

FReply FPythonSlateDelegate::OnClicked()
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
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

void FPythonSlateDelegate::OnTextChanged(const FText& text)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"s", TCHAR_TO_UTF8(*text.ToString()));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnStringChanged(const FString& text)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"s", TCHAR_TO_UTF8(*text));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnTextCommitted(const FText& text, ETextCommit::Type commit_type)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"si", TCHAR_TO_UTF8(*text.ToString()), (int)commit_type);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnInt32Changed(int32 value)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"i", value);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnInt32Committed(int32 value, ETextCommit::Type commit_type)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"ii", value, (int)commit_type);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnFloatChanged(float value)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"f", value);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnLinearColorChanged(FLinearColor color)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_flinearcolor(color));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnWindowClosed(const TSharedRef<SWindow> &Window)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_swidget<ue_PySWindow>(StaticCastSharedRef<SWidget>(Window), &ue_PySWindowType));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnFloatCommitted(float value, ETextCommit::Type commit_type)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"fi", value, (int)commit_type);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::OnSort(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"isi", (int)SortPriority, TCHAR_TO_UTF8(*ColumnName.ToString()), (int)NewSortMode);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

void FPythonSlateDelegate::CheckBoxChanged(ECheckBoxState state)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"i", (int)state);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

#if WITH_EDITOR
void FPythonSlateDelegate::OnAssetDoubleClicked(const FAssetData& AssetData)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fassetdata(AssetData));
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void FPythonSlateDelegate::OnAssetSelected(const FAssetData& AssetData)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fassetdata(AssetData));
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void FPythonSlateDelegate::OnAssetChanged(const FAssetData& AssetData)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fassetdata(AssetData));
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

bool FPythonSlateDelegate::OnShouldFilterAsset(const FAssetData& AssetData)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fassetdata(AssetData));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return false;
	}
	bool bValue = PyObject_IsTrue(ret) ? true : false;
	Py_DECREF(ret);
	return bValue;
}

TSharedPtr<SWidget> FPythonSlateDelegate::OnGetAssetContextMenu(const TArray<FAssetData>& SelectedAssets)
{
	FScopePythonGIL gil;

	PyObject *py_list = PyList_New(0);
	for (FAssetData asset : SelectedAssets)
	{
		PyList_Append(py_list, py_ue_new_fassetdata(asset));
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_list);
	Py_DECREF(py_list);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return nullptr;
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(ret);
	if (!Widget.IsValid())
	{
		Py_DECREF(ret);
		PyErr_Clear();
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return nullptr;
	}
	TSharedRef<SWidget> RefWidget = Widget.ToSharedRef();
	Py_DECREF(ret);
	return RefWidget;
}

void FPythonSlateDelegate::MenuPyAssetBuilder(FMenuBuilder &Builder, TArray<FAssetData> SelectedAssets)
{
	FScopePythonGIL gil;

	PyObject *py_list = PyList_New(0);
	for (FAssetData asset : SelectedAssets)
	{
		PyList_Append(py_list, py_ue_new_fassetdata(asset));
	}

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"NO", py_ue_new_fmenu_builder(Builder), py_list);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}


TSharedRef<FExtender> FPythonSlateDelegate::OnExtendContentBrowserMenu(const TArray<FAssetData>& SelectedAssets)
{
	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuExtension((char *)"GetAssetActions", EExtensionHook::After, nullptr, FMenuExtensionDelegate::CreateSP(this, &FPythonSlateDelegate::MenuPyAssetBuilder, SelectedAssets));

	return Extender;
}

#endif


void FPythonSlateDelegate::SubMenuPyBuilder(FMenuBuilder &Builder)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fmenu_builder(Builder));
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}


TSharedRef<SWidget> FPythonSlateDelegate::OnGenerateWidget(TSharedPtr<FPythonItem> py_item)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_item.Get()->py_object);
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
		PyErr_Clear();
		return SNullWidget::NullWidget;
	}
	TSharedRef<SWidget> RefWidget = Widget.ToSharedRef();
	Py_DECREF(ret);
	return RefWidget;
}

TSharedRef<SWidget> FPythonSlateDelegate::OnGetMenuContent()
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"");
	if (!ret)
	{
		unreal_engine_py_log_error();
		return SNullWidget::NullWidget;
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(ret);
	if (!Widget.IsValid())
	{
		Py_DECREF(ret);
		PyErr_Clear();
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return SNullWidget::NullWidget;
	}
	TSharedRef<SWidget> RefWidget = Widget.ToSharedRef();
	Py_DECREF(ret);
	return RefWidget;
}

void FPythonSlateDelegate::OnSelectionChanged(TSharedPtr<FPythonItem> py_item, ESelectInfo::Type select_type)
{
	if (!py_item.IsValid())
	{
		return;
	}

	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"Oi", py_item.Get()->py_object, (int)select_type);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	Py_DECREF(ret);
}

TSharedPtr<SWidget> FPythonSlateDelegate::OnContextMenuOpening()
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return nullptr;
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(ret);
	if (!Widget.IsValid())
	{
		Py_DECREF(ret);
		PyErr_Clear();
		UE_LOG(LogPython, Error, TEXT("returned value is not a SWidget"));
		return nullptr;
	}
	TSharedRef<SWidget> RefWidget = Widget.ToSharedRef();
	Py_DECREF(ret);
	return RefWidget;
}

void FPythonSlateDelegate::SimpleExecuteAction()
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

void FPythonSlateDelegate::ExecuteAction(PyObject *py_obj)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"O", py_obj);
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	Py_XDECREF(ret);
}

FText FPythonSlateDelegate::GetterFText() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return FText();
	}
	PyObject *str = PyObject_Str(ret);
	if (!str)
	{
		unreal_engine_py_log_error();
		Py_DECREF(ret);
		return FText();
	}

	FText text = FText::FromString(UEPyUnicode_AsUTF8(str));
	Py_DECREF(str);
	Py_DECREF(ret);
	return text;
}

FString FPythonSlateDelegate::GetterFString() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return FString();
	}
	PyObject *str = PyObject_Str(ret);
	if (!str)
	{
		unreal_engine_py_log_error();
		Py_DECREF(ret);
		return FString();
	}

	FString fstr = UTF8_TO_TCHAR(UEPyUnicode_AsUTF8(str));
	Py_DECREF(str);
	Py_DECREF(ret);
	return fstr;
}

float FPythonSlateDelegate::GetterFloat() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret))
	{
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_float = PyNumber_Float(ret);
	float n = PyFloat_AsDouble(py_float);
	Py_DECREF(py_float);
	Py_DECREF(ret);
	return n;
}

TOptional<float> FPythonSlateDelegate::GetterTFloat() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret))
	{
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_float = PyNumber_Float(ret);
	float n = PyFloat_AsDouble(py_float);
	Py_DECREF(py_float);
	Py_DECREF(ret);
	return n;
}

int FPythonSlateDelegate::GetterInt() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}
	if (!PyNumber_Check(ret))
	{
		PyErr_SetString(PyExc_ValueError, "returned value is not a number");
		Py_DECREF(ret);
		return 0;
	}

	PyObject *py_int = PyNumber_Long(ret);
	int n = PyLong_AsLong(py_int);
	Py_DECREF(py_int);
	Py_DECREF(ret);
	return n;
}

bool FPythonSlateDelegate::GetterBool() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return 0;
	}
	if (PyObject_IsTrue(ret))
	{
		Py_DECREF(ret);
		return true;
	}

	Py_DECREF(ret);
	return false;
}

FVector2D FPythonSlateDelegate::GetterFVector2D() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return FVector2D();
	}

	if (!PyTuple_Check(ret))
	{
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a tuple");
		return FVector2D();
	}

	if (PyTuple_Size(ret) != 2)
	{
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a 2 items tuple");
		return FVector2D();
	}

	PyObject *first_item = PyTuple_GetItem(ret, 0);
	if (!PyNumber_Check(first_item))
	{
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "tuple does not contain numbers");
		return FVector2D();
	}

	PyObject *py_x = PyNumber_Float(first_item);
	float x = (float)PyFloat_AsDouble(py_x);

	Py_DECREF(py_x);

	PyObject *py_y = PyNumber_Float(PyTuple_GetItem(ret, 1));
	float y = (float)PyFloat_AsDouble(py_y);

	Py_DECREF(py_y);

	Py_DECREF(ret);
	return FVector2D(x, y);
}

FLinearColor FPythonSlateDelegate::GetterFLinearColor() const
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, nullptr);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return FLinearColor();
	}

	ue_PyFLinearColor *py_color = py_ue_is_flinearcolor(ret);

	if (!py_color)
	{
		Py_DECREF(ret);
		PyErr_SetString(PyExc_ValueError, "returned value is not a FLinearColor");
		return FLinearColor();
	}

	FLinearColor color = py_color->color;

	Py_DECREF(ret);
	return color;
}

TSharedRef<SDockTab> FPythonSlateDelegate::SpawnPythonTab(const FSpawnTabArgs &args)
{
	FScopePythonGIL gil;
	TSharedRef<SDockTab> dock_tab = SNew(SDockTab).TabRole(ETabRole::NomadTab);
	PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", ue_py_get_swidget(dock_tab));
	if (!ret)
	{
		unreal_engine_py_log_error();
	}
	else
	{
		Py_DECREF(ret);
	}
	return dock_tab;
}

TSharedRef<ITableRow> FPythonSlateDelegate::GenerateRow(TSharedPtr<FPythonItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	FScopePythonGIL gil;

	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);
	}

	if (ue_PySPythonMultiColumnTableRow *spython_multicolumn_table_row = py_ue_is_spython_multicolumn_table_row(ret))
	{
		return StaticCastSharedRef<SPythonMultiColumnTableRow>(spython_multicolumn_table_row->s_compound_widget.s_widget.Widget->AsShared());
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(ret);
	Py_DECREF(ret);
	if (Widget.IsValid())
	{
		return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable).Content()[Widget.ToSharedRef()];
	}

	PyErr_Clear();

	UE_LOG(LogPython, Error, TEXT("python callable did not return a SWidget"));
	return SNew(STableRow<TSharedPtr<FPythonItem>>, OwnerTable);

}

void FPythonSlateDelegate::GetChildren(TSharedPtr<FPythonItem> InItem, TArray<TSharedPtr<FPythonItem>>& OutChildren)
{
	PyObject *ret = PyObject_CallFunction(py_callable, (char*)"O", InItem.Get()->py_object);
	if (!ret)
	{
		unreal_engine_py_log_error();
		return;
	}
	PyObject *py_iterable = PyObject_GetIter(ret);
	if (!py_iterable || !PyIter_Check(py_iterable))
	{
		UE_LOG(LogPython, Error, TEXT("returned value is not iterable"));
		Py_XDECREF(py_iterable);
		Py_DECREF(ret);
		return;
	}

	while (PyObject *item = PyIter_Next(py_iterable))
	{
		Py_INCREF(item);
		OutChildren.Add(TSharedPtr<FPythonItem>(new FPythonItem(item)));
	}
	Py_DECREF(py_iterable);
	Py_DECREF(ret);
}

ue_PySWidget *ue_py_get_swidget(TSharedRef<SWidget> s_widget)
{
	ue_PySWidget *ret = nullptr;

	if (s_widget->GetType().Compare(FName("SWindow")) == 0)
	{
		return py_ue_new_swidget<ue_PySWindow>(s_widget, &ue_PySWindowType);
	}
	if (s_widget->GetType().Compare(FName("SDockTab")) == 0)
	{
		return py_ue_new_swidget<ue_PySDockTab>(s_widget, &ue_PySDockTabType);
	}
	else
	{
		return py_ue_new_swidget<ue_PySWidget>(s_widget, &ue_PySWidgetType);
	}

}

void ue_python_init_slate(PyObject *module)
{

	ue_python_init_swidget(module);
	ue_python_init_scompound_widget(module);
	ue_python_init_swindow(module);
	ue_python_init_sborder(module);
	ue_python_init_sbutton(module);
	ue_python_init_sleaf_widget(module);
	ue_python_init_stext_block(module);
	ue_python_init_seditable_text_box(module);
	ue_python_init_smulti_line_editable_text(module);
	ue_python_init_spanel(module);
	ue_python_init_sgrid_panel(module);
	ue_python_init_sbox_panel(module);
	ue_python_init_shorizontal_box(module);
	ue_python_init_svertical_box(module);
	ue_python_init_sviewport(module);
	ue_python_init_simage(module);
	ue_python_init_sdock_tab(module);
	ue_python_init_stable_view_base(module);
	ue_python_init_slist_view(module);
	ue_python_init_spython_list_view(module);
	ue_python_init_spython_multicolumn_table_row(module);
	ue_python_init_stree_view(module);
	ue_python_init_spython_tree_view(module);
	ue_python_init_ssplitter(module);
	ue_python_init_sheader_row(module);
	ue_python_init_scheck_box(module);
	ue_python_init_snumeric_entry_box(module);
	ue_python_init_scanvas(module);
	ue_python_init_sslider(module);
	ue_python_init_svector_input_box(module);
	ue_python_init_srotator_input_box(module);
	ue_python_init_spython_combo_box(module);
	ue_python_init_sscroll_box(module);
	ue_python_init_scolor_block(module);
	ue_python_init_sbox(module);
	ue_python_init_sprogress_bar(module);
	ue_python_init_sspacer(module);
	ue_python_init_spython_widget(module);
	ue_python_init_soverlay(module);


#if WITH_EDITOR
	ue_python_init_snode_panel(module);
#if ENGINE_MINOR_VERSION > 15
	ue_python_init_sgraph_panel(module);
#endif
	ue_python_init_idetails_view(module);
	ue_python_init_istructure_details_view(module);
	ue_python_init_seditor_viewport(module);
	ue_python_init_slevel_viewport(module);
	ue_python_init_spython_editor_viewport(module);
	ue_python_init_sgraph_editor(module);
#if ENGINE_MINOR_VERSION > 14
	ue_python_init_spython_shelf(module);
#if ENGINE_MINOR_VERSION > 13
	ue_python_init_sfile_path_picker(module);
	ue_python_init_sdirectory_picker(module);
#endif
#endif
	ue_python_init_sdrop_target(module);
	ue_python_init_sasset_drop_target(module);
	ue_python_init_sobject_property_entry_box(module);
#endif

	ue_python_init_ftab_manager(module);
	ue_python_init_ftab_spawner_entry(module);
	ue_python_init_fmenu_builder(module);
	ue_python_init_fslate_style_set(module);
	ue_python_init_ftool_bar_builder(module);
	ue_python_init_fslate_icon(module);

	ue_python_init_fgeometry(module);
	ue_python_init_fpaint_context(module);

	ue_python_init_finput_event(module);
	ue_python_init_fpointer_event(module);
	ue_python_init_fkey_event(module);
	ue_python_init_fcharacter_event(module);
	ue_python_init_fmodifier_keys_state(module);
	ue_python_init_eslate_enums(module);
}

PyObject *ue_py_dict_get_item(PyObject *dict, const char *key)
{
	if (dict == nullptr)
		return nullptr;
	return PyDict_GetItemString(dict, key);
}

PyObject *py_unreal_engine_get_editor_window(PyObject *self, PyObject *args)
{

	if (!FGlobalTabmanager::Get()->GetRootWindow().IsValid())
	{
		return PyErr_Format(PyExc_Exception, "no RootWindow found");
	}

	return (PyObject *)ue_py_get_swidget(FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
}

PyObject *py_unreal_engine_find_slate_style(PyObject *self, PyObject *args)
{
	char *name = nullptr;
	if (!PyArg_ParseTuple(args, "s:find_slate_style", &name))
		return nullptr;

	ISlateStyle const* const foundStyleSet = FSlateStyleRegistry::FindSlateStyle(FName(name));
	if (!foundStyleSet)
	{
		UE_LOG(LogPython, Warning, TEXT("Could not find SlateStyle"));
		Py_RETURN_NONE;
	}

	ue_PyFSlateStyleSet *ret = py_ue_new_fslate_style_set(static_cast<FSlateStyleSet*>(const_cast<ISlateStyle*>(foundStyleSet)));
	return (PyObject *)ret;
}

PyObject *py_unreal_engine_find_icon_for_class(PyObject *self, PyObject *args)
{
	PyObject* py_class = nullptr;
	char *name = nullptr;
	if (!PyArg_ParseTuple(args, "O|s:find_icon_for_class", &py_class, &name))
		return nullptr;

	UClass *u_class = ue_py_check_type<UClass>(py_class);
	if (!u_class)
		return PyErr_Format(PyExc_Exception, "argument is not a UClass object");

	FSlateIcon foundIcon = FSlateIconFinder::FindIconForClass(u_class, FName(UTF8_TO_TCHAR(name)));
	ue_PyFSlateIcon *ret = py_ue_new_fslate_icon(foundIcon);
	return (PyObject *)ret;
}


// slate commands tool class
class FPythonSlateCommands : public TCommands<FPythonSlateCommands>
{
public:

	FPythonSlateCommands()
		: TCommands<FPythonSlateCommands>(TEXT("UnrealEnginePython"), NSLOCTEXT("Contexts", "UnrealEnginePython", "UnrealEnginePython"), NAME_None, "EditorStyle")
	{
	}

	void Setup(char *command_name, PyObject *py_object)
	{
		py_callable = py_object;
		Py_INCREF(py_callable);

		name = FString(command_name);
	}

	// TCommands<> interface
	virtual void RegisterCommands() override
	{
		commands = MakeShareable(new FUICommandList);
#if ENGINE_MINOR_VERSION >= 23
		MakeUICommand_InternalUseOnly(this, command, nullptr, *name, *name, TCHAR_TO_UTF8(*name), *name, *name, EUserInterfaceActionType::Button, FInputGesture());
#else
		UI_COMMAND_Function(this, command, nullptr, *name, *name, TCHAR_TO_UTF8(*name), *name, *name, EUserInterfaceActionType::Button, FInputGesture());
#endif
		commands->MapAction(command, FExecuteAction::CreateRaw(this, &FPythonSlateCommands::Callback), FCanExecuteAction());
	}

	void Callback()
	{
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallObject(py_callable, nullptr);
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	void MenuBuilder(FMenuBuilder &Builder)
	{
		Builder.AddMenuEntry(command);
	}

	void MenuPyBuilder(FMenuBuilder &Builder)
	{
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_fmenu_builder(Builder));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	void MenuBarBuilder(FMenuBarBuilder &Builder)
	{
		FText label = FText::FromString(name);
		Builder.AddPullDownMenu(label, label, FNewMenuDelegate::CreateRaw(this, &FPythonSlateCommands::MenuPyBuilder));
	}

	void ToolBarBuilder(FToolBarBuilder &Builder)
	{
		FScopePythonGIL gil;
		PyObject *ret = PyObject_CallFunction(py_callable, (char *)"N", py_ue_new_ftool_bar_builder(Builder));
		if (!ret)
		{
			unreal_engine_py_log_error();
			return;
		}
		Py_DECREF(ret);
	}

	TSharedPtr<FUICommandList> GetCommands()
	{
		return commands;
	}
private:
	TSharedPtr<FUICommandList> commands;
	TSharedPtr<FUICommandInfo> command;
	PyObject *py_callable;

	FString name;
};

#if WITH_EDITOR
PyObject *py_unreal_engine_create_detail_view(PyObject *self, PyObject * args, PyObject *kwargs)
{

	PyObject *py_object;
	PyObject *py_allow_search = nullptr;

	PyObject *py_update_from_selection = nullptr;
	PyObject *py_lockable = nullptr;
	char     *py_name_area_settings = nullptr;
	PyObject *py_hide_selection_tip = nullptr;
	PyObject *py_search_initial_key_focus = nullptr;

	char *kwlist[] = {
		(char *)"uobject",
		(char *)"allow_search",

		(char *)"update_from_selection",
		(char *)"lockable",
		(char *)"name_area_settings",
		(char *)"hide_selection_tip",
		(char *)"search_initial_key_focus",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOsOO:create_detail_view", kwlist,
		&py_object, &py_allow_search, &py_update_from_selection, &py_lockable, &py_name_area_settings, &py_hide_selection_tip, &py_search_initial_key_focus))
	{
		return nullptr;
	}


	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs view_args;
	view_args.bAllowSearch = (py_allow_search) ? PyObject_IsTrue(py_allow_search) : view_args.bAllowSearch;
	view_args.bUpdatesFromSelection = (py_update_from_selection) ? PyObject_IsTrue(py_update_from_selection) : view_args.bUpdatesFromSelection;
	view_args.bLockable = (py_lockable) ? PyObject_IsTrue(py_lockable) : view_args.bLockable;
	view_args.bHideSelectionTip = (py_hide_selection_tip) ? PyObject_IsTrue(py_hide_selection_tip) : view_args.bHideSelectionTip;
	view_args.bSearchInitialKeyFocus = (py_search_initial_key_focus) ? PyObject_IsTrue(py_search_initial_key_focus) : view_args.bSearchInitialKeyFocus;
	FString name_area_string = py_name_area_settings ? FString(UTF8_TO_TCHAR(py_name_area_settings)) : FString();
	view_args.NameAreaSettings = [&name_area_string]() {
		if (FCString::Stricmp(*name_area_string, TEXT("HideNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::HideNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ObjectsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ObjectsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ComponentsAndActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ComponentsAndActorsUseNameArea; }
		else { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
	}();

	TSharedPtr<IDetailsView> view = PropertyEditorModule.CreateDetailView(view_args);

	if (UObject *u_object = ue_py_check_type<UObject>(py_object))
	{
		view->SetObject(u_object);
	}

	extern PyTypeObject ue_PyIDetailsViewType;
	return (PyObject *)py_ue_new_swidget<ue_PyIDetailsView>(view->AsShared(), &ue_PyIDetailsViewType);
}

PyObject *py_unreal_engine_create_structure_detail_view(PyObject *self, PyObject * args, PyObject *kwargs)
{
	PyObject *py_object = nullptr;

	PyObject *py_allow_search = nullptr;
	PyObject *py_update_from_selection = nullptr;
	PyObject *py_lockable = nullptr;
	char     *py_name_area_settings = nullptr;
	PyObject *py_hide_selection_tip = nullptr;
	PyObject *py_search_initial_key_focus = nullptr;

	char *kwlist[] = {
		(char*)"struct_data",
		(char *)"allow_search",
		(char *)"update_from_selection",
		(char *)"lockable",
		(char *)"name_area_settings",
		(char *)"hide_selection_tip",
		(char *)"search_initial_key_focus",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|OOOOsOO:create_structure_detail_view", kwlist,
		&py_object, &py_allow_search, &py_update_from_selection, &py_lockable, &py_name_area_settings, &py_hide_selection_tip, &py_search_initial_key_focus))
	{
		return nullptr;
	}

	if (py_object && !py_ue_is_uscriptstruct(py_object))
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UScriptStruct");
	}

	FDetailsViewArgs view_args;
	view_args.bAllowSearch = (py_allow_search) ? PyObject_IsTrue(py_allow_search) : view_args.bAllowSearch;
	view_args.bUpdatesFromSelection = (py_update_from_selection) ? PyObject_IsTrue(py_update_from_selection) : view_args.bUpdatesFromSelection;
	view_args.bLockable = (py_lockable) ? PyObject_IsTrue(py_lockable) : view_args.bLockable;
	view_args.bHideSelectionTip = (py_hide_selection_tip) ? PyObject_IsTrue(py_hide_selection_tip) : view_args.bHideSelectionTip;
	view_args.bSearchInitialKeyFocus = (py_search_initial_key_focus) ? PyObject_IsTrue(py_search_initial_key_focus) : view_args.bSearchInitialKeyFocus;

	FString name_area_string = py_name_area_settings ? FString(UTF8_TO_TCHAR(py_name_area_settings)) : FString();
	view_args.NameAreaSettings = [&name_area_string]() {
		if (FCString::Stricmp(*name_area_string, TEXT("HideNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::HideNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ObjectsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ObjectsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
		else if (FCString::Stricmp(*name_area_string, TEXT("ComponentsAndActorsUseNameArea")) == 0) { return FDetailsViewArgs::ENameAreaSettings::ComponentsAndActorsUseNameArea; }
		else { return FDetailsViewArgs::ENameAreaSettings::ActorsUseNameArea; }
	}();
	FStructureDetailsViewArgs struct_view_args;
	{
		struct_view_args.bShowObjects = true;
		struct_view_args.bShowAssets = true;
		struct_view_args.bShowClasses = true;
		struct_view_args.bShowInterfaces = true;
	}

	extern PyTypeObject ue_PyIStructureDetailsViewType;
	ue_PyIStructureDetailsView *ret = (ue_PyIStructureDetailsView *)PyObject_New(ue_PyIStructureDetailsView, &ue_PyIStructureDetailsViewType);
	new(&ret->istructure_details_view) TSharedPtr<IStructureDetailsView>(nullptr);
	ret->ue_py_struct = nullptr;
	TSharedPtr<FStructOnScope> struct_scope;

	if (ue_PyUScriptStruct *ue_py_struct = py_ue_is_uscriptstruct(py_object))
	{
		Py_INCREF(ue_py_struct);
		ret->ue_py_struct = ue_py_struct;
		struct_scope = MakeShared<FStructOnScope>(ue_py_struct->u_struct, ue_py_struct->u_struct_ptr);
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	ret->istructure_details_view = PropertyEditorModule.CreateStructureDetailView(view_args, struct_view_args, struct_scope);

	return (PyObject *)ret;
}

PyObject *py_unreal_engine_create_property_view(PyObject *self, PyObject * args, PyObject *kwargs)
{

	PyObject *py_object;
	char *name;
	char *name_override = nullptr;

	char *kwlist[] = {
		(char*)"uobject",
		(char *)"name",
		(char *)"name_override",
		nullptr };

	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Os|s:create_property_view", kwlist,
		&py_object, &name, &name_override))
	{
		return nullptr;
	}

	UObject *u_object = ue_py_check_type<UObject>(py_object);
	if (!u_object)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a UObject");
	}

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FSinglePropertyParams params;
	if (name_override)
		params.NameOverride = FText::FromString(UTF8_TO_TCHAR(name_override));

	auto view_widget = PropertyEditorModule.CreateSingleProperty(u_object, FName(name), params);

	if (!view_widget.IsValid())
		return PyErr_Format(PyExc_Exception, "unable to create SingleProperty widget");

	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(view_widget->AsShared(), &ue_PySWidgetType);
}


PyObject *py_unreal_engine_add_menu_extension(PyObject * self, PyObject * args)
{

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"WindowLayout";
	char *module = (char*)"LevelEditor";

	if (!PyArg_ParseTuple(args, "sO|ss:add_menu_extension", &command_name, &py_callable, &hook, &module))
	{
		return NULL;
	}

	if (!FModuleManager::Get().ModuleExists(UTF8_TO_TCHAR(module)))
		return PyErr_Format(PyExc_Exception, "module %s does not exist", module);

	// unfortunately we need to manually check for module names :(
	IHasMenuExtensibility *menu_extension_interface = nullptr;

	if (!strcmp(module, (char *)"LevelEditor"))
	{
		FLevelEditorModule &Module = FModuleManager::LoadModuleChecked<FLevelEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
	else if (!strcmp(module, (char *)"Persona"))
	{
		FPersonaModule &Module = FModuleManager::LoadModuleChecked<FPersonaModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
#if ENGINE_MINOR_VERSION > 13
	else if (!strcmp(module, (char *)"AnimationEditor"))
	{
		IAnimationEditorModule &Module = FModuleManager::LoadModuleChecked<IAnimationEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}
#endif
	else if (!strcmp(module, (char *)"StaticMeshEditor"))
	{
		IStaticMeshEditorModule &Module = FModuleManager::LoadModuleChecked<IStaticMeshEditorModule>(module);
		menu_extension_interface = (IHasMenuExtensibility *)&Module;
	}

	if (!menu_extension_interface)
		return PyErr_Format(PyExc_Exception, "module %s is not supported", module);

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddMenuExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FMenuExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::MenuBuilder));

	menu_extension_interface->GetMenuExtensibilityManager()->AddExtender(extender);

	Py_RETURN_NONE;
}


PyObject *py_unreal_engine_add_menu_bar_extension(PyObject * self, PyObject * args)
{

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"Help";

	if (!PyArg_ParseTuple(args, "sO|s:add_menu_bar_extension", &command_name, &py_callable, &hook))
	{
		return NULL;
	}

	FLevelEditorModule &ExtensibleModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");


	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddMenuBarExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FMenuBarExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::MenuBarBuilder));

	ExtensibleModule.GetMenuExtensibilityManager()->AddExtender(extender);

	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_add_tool_bar_extension(PyObject * self, PyObject * args)
{

	char *command_name;
	PyObject *py_callable;

	char *hook = (char *)"Settings";

	if (!PyArg_ParseTuple(args, "sO|s:add_tool_bar_extension", &command_name, &py_callable, &hook))
	{
		return NULL;
	}

	FLevelEditorModule &ExtensibleModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	TSharedRef<FPythonSlateCommands> *commands = new TSharedRef<FPythonSlateCommands>(new FPythonSlateCommands());

	commands->Get().Setup(command_name, py_callable);

	commands->Get().RegisterCommands();

	TSharedRef<FExtender> extender = MakeShareable(new FExtender());
	extender->AddToolBarExtension(hook, EExtensionHook::After, commands->Get().GetCommands(), FToolBarExtensionDelegate::CreateRaw(&commands->Get(), &FPythonSlateCommands::ToolBarBuilder));

	ExtensibleModule.GetToolBarExtensibilityManager()->AddExtender(extender);

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_add_asset_view_context_menu_extension(PyObject * self, PyObject * args)
{

	PyObject *py_callable;

	if (!PyArg_ParseTuple(args, "O:add_asset_view_context_menu_extension", &py_callable))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	FContentBrowserModule &ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FContentBrowserMenuExtender_SelectedAssets> &Extenders = ContentBrowser.GetAllAssetViewContextMenuExtenders();

	FContentBrowserMenuExtender_SelectedAssets handler;
	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);
	handler.BindSP(py_delegate, &FPythonSlateDelegate::OnExtendContentBrowserMenu);

	Extenders.Add(handler);

	Py_RETURN_NONE;
}
#endif

PyObject *py_unreal_engine_register_nomad_tab_spawner(PyObject * self, PyObject * args)
{

	char *name;
	PyObject *py_callable;
	PyObject *py_icon = nullptr;
	if (!PyArg_ParseTuple(args, "sO|O:register_nomad_tab_spawner", &name, &py_callable, &py_icon))
	{
		return NULL;
	}

	if (!PyCallable_Check(py_callable))
		return PyErr_Format(PyExc_Exception, "argument is not callable");

	FOnSpawnTab spawn_tab;
	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);
	spawn_tab.BindSP(py_delegate, &FPythonSlateDelegate::SpawnPythonTab);

	FName TabName = FName(UTF8_TO_TCHAR(name));

	// avoid crash if re-registering the same tab
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabName);

	FSlateIcon Icon = FSlateIcon();
	if (py_icon)
	{
		ue_PyFSlateIcon *slate_icon = py_ue_is_fslate_icon(py_icon);
		if (!slate_icon)
		{
			return PyErr_Format(PyExc_Exception, "argument is not a FSlateIcon");
		}
		Icon = slate_icon->icon;
	}

	FTabSpawnerEntry *SpawnerEntry = &FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabName, spawn_tab)
		.SetDisplayName(FText::FromString((TabName).ToString()))
		.SetTooltipText(FText::FromString((TabName).ToString()))
		.SetIcon(Icon)
		// TODO: more generic way to set the group
#if WITH_EDITOR
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
#endif
		;

	PyObject *ret = py_ue_new_ftab_spawner_entry(SpawnerEntry);
	Py_INCREF(ret);
	return ret;
}

PyObject *py_unreal_engine_unregister_nomad_tab_spawner(PyObject * self, PyObject * args)
{

	char *name;
	if (!PyArg_ParseTuple(args, "s:unregister_nomad_tab_spawner", &name))
	{
		return NULL;
	}

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UTF8_TO_TCHAR(name));

	Py_INCREF(Py_None);
	return Py_None;
}

PyObject *py_unreal_engine_invoke_tab(PyObject * self, PyObject * args)
{

	char *name;
	if (!PyArg_ParseTuple(args, "s:invoke_tab", &name))
	{
		return NULL;
	}

	FGlobalTabmanager::Get()->InvokeTab(FTabId(FName(UTF8_TO_TCHAR(name))));

	Py_INCREF(Py_None);
	return Py_None;
}


PyObject * py_unreal_engine_get_swidget_from_wrapper(PyObject *self, PyObject *args)
{
	PyObject *py_object;

	if (!PyArg_ParseTuple(args, "O:get_swidget_from_wrapper", &py_object))
	{
		return NULL;
	}

	FPythonSWidgetWrapper *py_swidget_wrapper = ue_py_check_struct<FPythonSWidgetWrapper>(py_object);
	if (!py_swidget_wrapper)
		return PyErr_Format(PyExc_Exception, "argument is not a FPythonSWidgetWrapper");

	if (!py_swidget_wrapper->Widget.IsValid())
		return PyErr_Format(PyExc_Exception, "wrapper contained invalid SWidget!");

	return (PyObject *)py_ue_new_swidget<ue_PySWidget>(py_swidget_wrapper->Widget->AsShared(), &ue_PySWidgetType);
}

PyObject * py_unreal_engine_create_wrapper_from_pyswidget(PyObject *self, PyObject *args)
{
	PyObject *py_object;

	if (!PyArg_ParseTuple(args, "O:create_wrapper_from_pyswidget", &py_object))
	{
		return NULL;
	}

	TSharedPtr<SWidget> Widget = py_ue_is_swidget<SWidget>(py_object);
	if (!Widget.IsValid())
	{
		return nullptr;
	}

	FPythonSWidgetWrapper py_swidget_wrapper;
	py_swidget_wrapper.Widget = Widget;
	return py_ue_new_owned_uscriptstruct(FPythonSWidgetWrapper::StaticStruct(), (uint8 *)&py_swidget_wrapper);
}

PyObject *py_unreal_engine_open_color_picker(PyObject *self, PyObject *args, PyObject *kwargs)
{

	PyObject *py_callable = nullptr;

	char *kwlist[] = {
		(char *)"on_color_committed",
		nullptr };
	if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O:open_color_picker", kwlist,
		&py_callable))
	{
		return nullptr;
	}

	if (!PyCallable_Check(py_callable))
	{
		return PyErr_Format(PyExc_Exception, "on_color_committed must be a callable");
	}

	TSharedRef<FPythonSlateDelegate> py_delegate = FUnrealEnginePythonHouseKeeper::Get()->NewStaticSlateDelegate(py_callable);
	FColorPickerArgs color_args;
	color_args.OnColorCommitted.BindSP(py_delegate, &FPythonSlateDelegate::OnLinearColorChanged);

	if (OpenColorPicker(color_args))
	{
		Py_RETURN_TRUE;
	}
	Py_RETURN_FALSE;
}

PyObject *py_unreal_engine_destroy_color_picker(PyObject *self, PyObject * args)
{
	DestroyColorPicker();
	Py_RETURN_NONE;
}

PyObject *py_unreal_engine_play_sound(PyObject *self, PyObject * args)
{
	PyObject *py_sound;
	int user_index;
	if (!PyArg_ParseTuple(args, "O|i:play_sound", &py_sound, &user_index))
	{
		return nullptr;
	}

	FSlateSound *sound = ue_py_check_struct<FSlateSound>(py_sound);
	if (!sound)
	{
		USoundBase *u_sound = ue_py_check_type<USoundBase>(py_sound);
		if (u_sound)
		{
			FSlateSound slate_sound = FSlateSound();
			slate_sound.SetResourceObject(u_sound);
			sound = &slate_sound;
		}
	}

	if (!sound)
	{
		return PyErr_Format(PyExc_Exception, "argument is not a FSlateColor or a USoundBase");
	}

	FSlateApplication::Get().PlaySound(*sound, user_index);
	Py_RETURN_NONE;
}
