// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "SPythonEditableText.h"
#include "PythonEditorStyle.h"
#include "Runtime/Slate/Public/Framework/Application/SlateApplication.h"


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
		.OnCursorMoved(this, &SPythonEditableText::OnCursorMoved)
	);
	OnExecuted = InArgs._OnExecuted;
	CurrentScale = 1;
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
	// substitute tab, with 4 spaces
	if (Character == TEXT('\t'))
	{
		InsertTextAtCursor(FString("    "));
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

FReply SPythonEditableText::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& InPointerEvent)
{
	if (FSlateApplication::Get().GetModifierKeys().IsControlDown())
	{
		if (InPointerEvent.GetWheelDelta() > 0)
		{
			CurrentScale += 0.1;
		}
		else if (InPointerEvent.GetWheelDelta() < 0)
		{
			CurrentScale -= 0.1;
		}

		if (CurrentScale < 1)
			CurrentScale = 1;
		SetRenderTransform(FSlateRenderTransform(CurrentScale));
		return FReply::Handled();
	}
	return SMultiLineEditableText::OnMouseWheel(MyGeometry, InPointerEvent);
}

FReply SPythonEditableText::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Reply = FReply::Unhandled();
	if (InKeyEvent.GetKeyCode() == 9)
	{
		Reply = FReply::Handled();
	}
	else if (InKeyEvent.IsControlDown() && InKeyEvent.GetKeyCode() == 13)
	{
		Reply = FReply::Handled();
		OnExecuted.Execute();
	}
	else
	{
		Reply = SMultiLineEditableText::OnKeyDown(MyGeometry, InKeyEvent);
	}

	//FString test;
	//test=FString::FromInt(InKeyEvent.GetKeyCode());
	//InsertTextAtCursor(test);

	return Reply;
}

void SPythonEditableText::GetLineAndColumn(int32 & Line, int32 & Column)
{
	Line = CurrentLine;
	Column = CurrentColumn;
}

