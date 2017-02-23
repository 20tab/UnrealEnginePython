// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonEditorPrivatePCH.h"
#include "SPythonEditableText.h"


void SPythonEditableText::Construct(const FArguments& InArgs)
{

	SMultiLineEditableText::Construct(
		SMultiLineEditableText::FArguments()
		.Font(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("TextEditor.NormalText").Font)
		.TextStyle(&FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("TextEditor.NormalText"))
		.Text(InArgs._Text)
		.Marshaller(InArgs._Marshaller)
		.AutoWrapText(false)
		.Margin(0.0f)
		.HScrollBar(InArgs._HScrollBar)
		.VScrollBar(InArgs._VScrollBar)
		.OnTextChanged(InArgs._OnTextChanged)
		);
	OnExecuted = InArgs._OnExecuted;
}

FReply SPythonEditableText::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& InCharacterEvent)
{
	FReply Reply = FReply::Unhandled();

	const TCHAR Character = InCharacterEvent.GetCharacter();
	if (IsTextReadOnly())
	{
		return Reply;
	}
	Reply = FReply::Handled();
	if (Character == TEXT('\t'))
	{
		FString String;
		String.AppendChar(Character);
		InsertTextAtCursor(String);
	}
	//else if (Character == TEXT('('))
	//{
	//	FString String=TEXT("()");
	//	InsertTextAtCursor(String);
	//}
	//else if (Character == TEXT('['))
	//{
	//	FString String = TEXT("[]");
	//	InsertTextAtCursor(String);
	//}
	//else if (Character == TEXT('{'))
	//{
	//	FString String = TEXT("{}");
	//	InsertTextAtCursor(String);
	//}
	//else if (Character == TEXT('\''))
	//{
	//	FString String = TEXT("''");
	//	InsertTextAtCursor(String);
	//}
	//else if (Character == TEXT('"'))
	//{
	//	FString String = TEXT("\"\"");
	//	InsertTextAtCursor(String);
	//}
	else
	{
		Reply = SMultiLineEditableText::OnKeyChar(MyGeometry, InCharacterEvent);
	}

	return Reply;
}

FReply SPythonEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();
	if (InKeyEvent.GetKeyCode() == 9) {
		Reply = FReply::Handled();
	}
	else if (InKeyEvent.IsControlDown() && InKeyEvent.GetKeyCode() == 13) {
		Reply = FReply::Handled();
		OnExecuted.Execute();
	}
	else {
		Reply = SMultiLineEditableText::OnKeyDown(MyGeometry, InKeyEvent);
	}

	//FString test;
	//test=FString::FromInt(InKeyEvent.GetKeyCode());
	//InsertTextAtCursor(test);

	return Reply;
}

