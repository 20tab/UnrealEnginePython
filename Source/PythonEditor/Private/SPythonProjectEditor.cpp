// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SPythonProjectEditor.h"
#include "SProjectViewItem.h"
#include "DirectoryScanner.h"
#include "Runtime/Slate/Public/Widgets/Images/SThrobber.h"
#include "PythonEditorStyle.h"
#include "PythonProjectEditor.h"
#include "PythonProject.h"

#define LOCTEXT_NAMESPACE "PythonProjectEditor"


void SPythonProjectEditor::Construct(const FArguments& InArgs, UPythonProject* InPythonProject)
{
	check(InPythonProject);
	PythonProject = InPythonProject;

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FPythonEditorStyle::Get().GetBrush("ProjectEditor.Border"))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			SAssignNew(ProjectTree, STreeView<UPythonProjectItem*>)
			.TreeItemsSource(&PythonProject->Children)
		.OnGenerateRow(this, &SPythonProjectEditor::OnGenerateRow)
		.OnGetChildren(this, &SPythonProjectEditor::OnGetChildren)
		.OnMouseButtonDoubleClick(this, &SPythonProjectEditor::HandleMouseButtonDoubleClick)
		.ClearSelectionOnClick(false)
		]
	+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.Padding(10.0f)
		[
			SNew(SThrobber)
			.Visibility(this, &SPythonProjectEditor::GetThrobberVisibility)
		]
		]
		];

	InPythonProject->RescanChildren();
}

void SPythonProjectEditor::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (FDirectoryScanner::Tick())
	{
		ProjectTree->SetTreeItemsSource(&PythonProject->Children);
	}

	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FName SPythonProjectEditor::GetIconForItem(UPythonProjectItem* Item) const
{
	switch (Item->Type)
	{
	case EPythonProjectItemType::Project:
		return "ProjectEditor.Icon.Project";
	case EPythonProjectItemType::Folder:
		return "ProjectEditor.Icon.Folder";
	case EPythonProjectItemType::File:
		return "ProjectEditor.Icon.File";
	default:
		return "ProjectEditor.Icon.GenericFile";
	}
}

TSharedRef<class ITableRow> SPythonProjectEditor::OnGenerateRow(UPythonProjectItem* Item, const TSharedRef<STableViewBase >& OwnerTable)
{
	return
		SNew(STableRow<UPythonProjectItem*>, OwnerTable)
		[
			SNew(SProjectViewItem)
			.TreeItem(Item)
		.Text(FText::FromString(Item->Name))
		.IconName(GetIconForItem(Item))
		.IsSelected(this, &SPythonProjectEditor::IsTreeItemSelected, Item)
		.OnNameChanged(this, &SPythonProjectEditor::FolderNameChanged)
		];
}
UPythonProjectItem* SPythonProjectEditor::SelectByPath(FString path)
{
	ProjectTree->SetTreeItemsSource(&PythonProject->Children);
	UPythonProjectItem* Item = PythonProject->GetItemByPath(path);
	if (Item != NULL) {
		ProjectTree->ClearSelection();
		ProjectTree->SetItemSelection(Item, true);
	}
	return Item;
}
void SPythonProjectEditor::RequestTreeRefresh()
{
	ProjectTree->RequestTreeRefresh();
}
bool SPythonProjectEditor::HasSelectionItem()
{
	int32 selNum = ProjectTree->GetNumItemsSelected();
	return selNum > 0;
}
TArray<UPythonProjectItem*> SPythonProjectEditor::GetSelectedItems()
{
	TArray<UPythonProjectItem*> selItemList = ProjectTree->GetSelectedItems();
	return selItemList;
}

void SPythonProjectEditor::FolderNameChanged(UPythonProjectItem* Item)
{
	if (Item->Type == EPythonProjectItemType::File) {
		FPythonProjectEditor::Get()->CloseFileForEditing(Item);
		FPythonProjectEditor::Get()->OpenFileForEditing(Item);
	}
}
bool SPythonProjectEditor::IsTreeItemSelected(UPythonProjectItem* Item) const
{
	return ProjectTree->IsItemSelected(Item);
}

void SPythonProjectEditor::OnGetChildren(UPythonProjectItem* Item, TArray<UPythonProjectItem*>& OutChildItems)
{
	OutChildItems = Item->Children;
}

EVisibility SPythonProjectEditor::GetThrobberVisibility() const
{
	return FDirectoryScanner::IsScanning() ? EVisibility::Visible : EVisibility::Hidden;
}

void SPythonProjectEditor::HandleMouseButtonDoubleClick(UPythonProjectItem* Item) const
{
	if (Item->Type == EPythonProjectItemType::File)
	{
		FPythonProjectEditor::Get()->OpenFileForEditing(Item);
	}
}

#undef LOCTEXT_NAMESPACE