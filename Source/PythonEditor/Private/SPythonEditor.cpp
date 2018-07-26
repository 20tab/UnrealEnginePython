// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SPythonEditor.h"
#include "Runtime/Slate/Public/Widgets/Text/SMultiLineEditableText.h"
#include "PYRichTextSyntaxHighlighterTextLayoutMarshaller.h"
#include "SPythonEditableText.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "PythonProjectItem.h"
#include "Runtime/Slate/Public/Widgets/Layout/SGridPanel.h"
#include "PythonEditorStyle.h"
#include "Runtime/Slate/Public/Widgets/Text/STextBlock.h"
#include "Runtime/Slate/Public/Widgets/Layout/SScrollBar.h"
#include "UnrealEnginePython.h"


#define LOCTEXT_NAMESPACE "PythonEditor"

void SPythonEditor::Construct(const FArguments& InArgs, UPythonProjectItem* InPythonProjectItem)
{
	bDirty = false;

	check(InPythonProjectItem);
	PythonProjectItem = InPythonProjectItem;

	FString FileText = "File Loading, please wait";
	FFileHelper::LoadFileToString(FileText, *InPythonProjectItem->Path);

	TSharedRef<FPYRichTextSyntaxHighlighterTextLayoutMarshaller> RichTextMarshaller = FPYRichTextSyntaxHighlighterTextLayoutMarshaller::Create(
			FPYRichTextSyntaxHighlighterTextLayoutMarshaller::FSyntaxTextStyle()
			);

	HorizontalScrollbar = 
		SNew(SScrollBar)
		.Orientation(Orient_Horizontal)
		.Thickness(FVector2D(10.0f, 10.0f));

	VerticalScrollbar = 
		SNew(SScrollBar)
		.Orientation(Orient_Vertical)
		.Thickness(FVector2D(10.0f, 10.0f));

	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FPythonEditorStyle::Get().GetBrush("TextEditor.Border"))
		[
			SNew(SGridPanel)
			.FillColumn(0, 1.0f)
			.FillRow(0, 2.0f)
			+SGridPanel::Slot(0, 0)
			[
				SAssignNew(PythonEditableText, SPythonEditableText)
				.Text(FText::FromString(FileText))
				.Marshaller(RichTextMarshaller)
				.HScrollBar(HorizontalScrollbar)
				.VScrollBar(VerticalScrollbar)
				.OnTextChanged(this, &SPythonEditor::OnTextChanged)
				.OnExecuted(this,&SPythonEditor::Execute)
			]
			+SGridPanel::Slot(1, 0)
			[
				VerticalScrollbar.ToSharedRef()
			]
			+SGridPanel::Slot(0, 1)
			[
				HorizontalScrollbar.ToSharedRef()
			]
			+ SGridPanel::Slot(0, 2)
			[
				SNew(SBorder).HAlign(EHorizontalAlignment::HAlign_Right)
				[
					SNew(STextBlock)
					.Text(this, &SPythonEditor::GetLineAndColumn)
				]
			]
		]
		
	];
}

void SPythonEditor::OnTextChanged(const FText& NewText)
{
	bDirty = true;
}

bool SPythonEditor::Save() const
{
	if(bDirty)
	{
		bool bResult = FFileHelper::SaveStringToFile(PythonEditableText->GetText().ToString(), *PythonProjectItem->Path);
		if(bResult)
		{
			bDirty = false;
		}
		return bResult;
	}
	return true;
}

bool SPythonEditor::CanSave() const
{
	return bDirty;
}

void SPythonEditor::Execute() const
{
	Save();
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	
	FString SelectionString = PythonEditableText->GetSelectedText().ToString();
	if (SelectionString.Len() == 0) {
		SelectionString = PythonEditableText->GetText().ToString();
	}
	PythonModule.RunString(TCHAR_TO_UTF8(*SelectionString));
}

#if PLATFORM_MAC
void SPythonEditor::ExecuteInMainThread() const
{
	Save();
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");
	
	FString SelectionString = PythonEditableText->GetSelectedText().ToString();
	if (SelectionString.Len() == 0) {
		SelectionString = PythonEditableText->GetText().ToString();
	}
	PythonModule.RunStringInMainThread(TCHAR_TO_UTF8(*SelectionString));
}
#endif


void SPythonEditor::PEP8ize() const
{
	Save();
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");

	FString CleanedCode = PythonModule.Pep8ize(PythonEditableText->GetText().ToString());

	PythonEditableText->SetText(FText::FromString(CleanedCode));
}


void SPythonEditor::GotoLineAndColumn(int32 LineNumber, int32 ColumnNumber)
{
	FTextLocation Location(LineNumber, ColumnNumber);
	PythonEditableText->GoTo(Location);
	PythonEditableText->ScrollTo(Location);
}

FText SPythonEditor::GetLineAndColumn() const
{
	int32 Line;
	int32 Column;
	PythonEditableText->GetLineAndColumn(Line, Column);
	
	FString LineAndColumn = FString::Printf(TEXT("Line: %d Column: %d"), Line + 1, Column);
	
	return FText::FromString(LineAndColumn);
}

#undef LOCTEXT_NAMESPACE
