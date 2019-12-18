// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateCore.h"
#include "Runtime/Slate/Public/Widgets/Views/STreeView.h"
#include "PythonProjectItem.h"

class SPythonProjectEditor : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPythonProjectEditor) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, class UPythonProject* InPythonProject);

	UPythonProjectItem*  SelectByPath(FString path);
	bool HasSelectionItem();
	TArray<UPythonProjectItem*> GetSelectedItems();

	void RequestTreeRefresh();
private:
	/** Begin SWidget interface */
	void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;
	/** End SWidget interface */

	TSharedRef<class ITableRow> OnGenerateRow(class UPythonProjectItem* Item, const TSharedRef<class STableViewBase>& TableView);

	void OnGetChildren(class UPythonProjectItem* Item, TArray<class UPythonProjectItem*>& OutChildItems);

	EVisibility GetThrobberVisibility() const;

	FName GetIconForItem(class UPythonProjectItem* Item) const;

	void HandleMouseButtonDoubleClick(class UPythonProjectItem* Item) const;

	bool IsTreeItemSelected(class UPythonProjectItem* Item) const;

	void FolderNameChanged(UPythonProjectItem* Item);

	TSharedPtr<SWidget> MakePathViewContextMenu();
	

private:
	class UPythonProject* PythonProject;

	TSharedPtr<STreeView<class UPythonProjectItem*>> ProjectTree;
};