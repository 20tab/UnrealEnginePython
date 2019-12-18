// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UnrealEd.h"
#include "PythonProjectItem.generated.h"

/** Types of project items. Note that the enum ordering determines the tree sorting */
UENUM()
namespace EPythonProjectItemType
{
	enum Type
	{
		Project,
		Folder,
		File
	};
}

UCLASS()
class UPythonProjectItem : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	void RescanChildren();
	UPythonProjectItem* GetItemByPath(FString FullPath);

	void HandleDirectoryScanned(const FString& InPathName, EPythonProjectItemType::Type InType);

	/** Handle directory changing */
	void HandleDirectoryChanged(const TArray<struct FFileChangeData>& FileChanges);

public:
	UPROPERTY(Transient)
	TEnumAsByte<EPythonProjectItemType::Type> Type;

	UPROPERTY(Transient)
	FString Name;

	UPROPERTY(Transient)
	FString Extension;

	UPROPERTY(Transient)
	FString Path;

	UPROPERTY(Transient)
	TArray<UPythonProjectItem*> Children;

	/** Delegate handle for directory watcher */
	FDelegateHandle OnDirectoryChangedHandle;
};
