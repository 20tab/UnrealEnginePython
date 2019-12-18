// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonProjectItem.h"
#include "DirectoryScanner.h"
#include "Developer/DirectoryWatcher/Public/IDirectoryWatcher.h"

UPythonProjectItem::UPythonProjectItem(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPythonProjectItem::RescanChildren()
{
	if (Path.Len() > 0)
	{
		TArray<UPythonProjectItem*> DeleteChildren;
		for (const auto& Child : Children)
		{
			if (!FPaths::FileExists(Child->Path)) {
				DeleteChildren.Add(Child);
			}
		}
		for (const auto& Child : DeleteChildren)
		{
			Children.Remove(Child);
		}
		DeleteChildren.Empty();
		FDirectoryScanner::AddDirectory(Path, FOnDirectoryScanned::CreateUObject(this, &UPythonProjectItem::HandleDirectoryScanned));
		if (FDirectoryScanner::IsScanning()) {
			FDirectoryScanner::Tick();
		}
	}
}

UPythonProjectItem* UPythonProjectItem::GetItemByPath(FString FullPath)
{
	for (const auto& Child : Children)
	{
		if (Child->Type == EPythonProjectItemType::File && Child->Path == FullPath)
		{
			return Child;
		}
	}
	return NULL;
}

void UPythonProjectItem::HandleDirectoryScanned(const FString& InPathName, EPythonProjectItemType::Type InType)
{
	// check for a child that already exists
	bool bCreateNew = true;
	for (const auto& Child : Children)
	{
		if (Child->Type == InType && Child->Path == InPathName)
		{
			bCreateNew = false;
			break;
		}
	}

	// create children now & kick off their scan
	if (bCreateNew)
	{
		if (InType == EPythonProjectItemType::File && FPaths::GetExtension(InPathName) != TEXT("py")) {
			return;
		}
		UPythonProjectItem* NewItem = NewObject<UPythonProjectItem>(GetOutermost(), UPythonProjectItem::StaticClass());
		NewItem->Type = InType;
		NewItem->Path = InPathName;
		NewItem->Name = FPaths::GetBaseFilename(InPathName);
		if (InType != EPythonProjectItemType::Folder)
		{
			NewItem->Extension = FPaths::GetExtension(InPathName);
		}

		Children.Add(NewItem);

		Children.Sort(
			[](const UPythonProjectItem& ItemA, const UPythonProjectItem& ItemB) -> bool
		{
			if (ItemA.Type != ItemB.Type)
			{
				return ItemA.Type < ItemB.Type;
			}

			return ItemA.Name.Compare(ItemB.Name) < 0;
		}
		);

		if (InType == EPythonProjectItemType::Folder)
		{
			FDirectoryScanner::AddDirectory(InPathName, FOnDirectoryScanned::CreateUObject(NewItem, &UPythonProjectItem::HandleDirectoryScanned));
		}
	}
}

void UPythonProjectItem::HandleDirectoryChanged(const TArray<FFileChangeData>& FileChanges)
{
	// @TODO: dynamical update directory watchers so we can update the view in real-time
	for (const auto& Change : FileChanges)
	{
		switch (Change.Action)
		{
		default:
		case FFileChangeData::FCA_Unknown:
			break;
		case FFileChangeData::FCA_Added:
		{

		}
		break;
		case FFileChangeData::FCA_Modified:
		{

		}
		break;
		case FFileChangeData::FCA_Removed:
		{

		}
		break;
		}
	}
}