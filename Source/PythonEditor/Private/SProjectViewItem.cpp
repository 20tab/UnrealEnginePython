// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SProjectViewItem.h"
#include "Runtime/Slate/Public/Widgets/Text/SInlineEditableTextBlock.h"
#include "Runtime/Core/Public/Internationalization/BreakIterator.h"
#include "PythonEditorStyle.h"

#define LOCTEXT_NAMESPACE "ProjectViewItem"


void SProjectViewItem::Construct(const FArguments& InArgs)
{
	TreeItem = InArgs._TreeItem;
	OnNameChanged = InArgs._OnNameChanged;
	ChildSlot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(1.0f)
		.AutoWidth()
		[
			SNew(SImage)
			.Image(FPythonEditorStyle::Get().GetBrush(InArgs._IconName))
		]
	+ SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(1.0f)
		.FillWidth(1.0f)
		[
			SAssignNew(InlineRenameWidget, SInlineEditableTextBlock)
			.Text(InArgs._Text)
		.OnTextCommitted(this, &SProjectViewItem::HandleNameCommitted)
		.IsSelected(InArgs._IsSelected)
		]
		];
}

void SProjectViewItem::HandleNameCommitted(const FText& NewText, ETextCommit::Type)
{

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	const FString OldPath = TreeItem->Path;
	FString Path;
	TreeItem->Path.Split(TEXT("/"), &Path, NULL, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	FString NewPath = Path + TEXT("/") + NewText.ToString() + TEXT(".py");
	if (!FPaths::FileExists(NewPath)) {
		int Result = rename(TCHAR_TO_ANSI(*OldPath), TCHAR_TO_ANSI(*NewPath));
		if (Result == 0) {
			TreeItem->Name = NewText.ToString();
			TreeItem->Path = NewPath;
			InlineRenameWidget->SetText(TreeItem->Name);

			OnNameChanged.ExecuteIfBound(TreeItem);
		}
	}
}

#undef LOCTEXT_NAMESPACE
