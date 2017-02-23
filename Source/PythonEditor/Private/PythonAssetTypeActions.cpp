// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"
#include "PythonAssetTypeActions.h"


#define LOCTEXT_NAMESPACE "PythonAssetTypeActions"


FText FPythonAssetTypeActions::GetName() const
{
	return LOCTEXT("PythonProjectActionsName", "Python Project");
}

FColor FPythonAssetTypeActions::GetTypeColor() const
{
	return FColor(255, 255, 0);
}

UClass* FPythonAssetTypeActions::GetSupportedClass() const
{
	return UPythonProject::StaticClass();
}

void FPythonAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UPythonProject* PythonProject = Cast<UPythonProject>(*ObjIt))
		{
			TSharedRef<FPythonProjectEditor> NewPythonProjectEditor(new FPythonProjectEditor());
			NewPythonProjectEditor->InitPythonEditor(Mode, EditWithinLevelEditor, PythonProject);
		}
	}
}

uint32 FPythonAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}

#undef LOCTEXT_NAMESPACE