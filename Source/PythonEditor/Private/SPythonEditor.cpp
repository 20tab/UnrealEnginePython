// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"
#include "SPythonEditor.h"
#include "SMultiLineEditableText.h"
#include "PYRichTextSyntaxHighlighterTextLayoutMarshaller.h"
#include "SPythonEditableText.h"


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
			.FillRow(0, 1.0f)
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

void SPythonEditor::ExecuteInSandbox() const
{
	Save();
	FUnrealEnginePythonModule &PythonModule = FModuleManager::GetModuleChecked<FUnrealEnginePythonModule>("UnrealEnginePython");

	FString SelectionString = PythonEditableText->GetSelectedText().ToString();
	if (SelectionString.Len() == 0) {
		SelectionString = PythonEditableText->GetText().ToString();
	}
	PythonModule.RunStringSandboxed(TCHAR_TO_UTF8(*SelectionString));
}


void SPythonEditor::GotoLineAndColumn(int32 LineNumber, int32 ColumnNumber)
{
	FTextLocation Location(LineNumber, ColumnNumber);
	PythonEditableText->GoTo(Location);
	PythonEditableText->ScrollTo(Location);
}

#undef LOCTEXT_NAMESPACE