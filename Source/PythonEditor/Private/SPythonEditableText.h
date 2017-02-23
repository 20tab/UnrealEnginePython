// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SMultiLineEditableText.h"

class SPythonEditableText : public SMultiLineEditableText
{
	DECLARE_DELEGATE(FOnExecuted);
	SLATE_BEGIN_ARGS( SPythonEditableText )
	{}
		/** The initial text that will appear in the widget. */
		SLATE_ATTRIBUTE(FText, Text)

		/** The marshaller used to get/set the raw text to/from the text layout. */
		SLATE_ARGUMENT(TSharedPtr< ITextLayoutMarshaller >, Marshaller)

		/** The horizontal scroll bar widget */
		SLATE_ARGUMENT(TSharedPtr< SScrollBar >, HScrollBar)

		/** The vertical scroll bar widget */
		SLATE_ARGUMENT(TSharedPtr< SScrollBar >, VScrollBar)

		/** Called whenever the text is changed interactively by the user */
		SLATE_EVENT(FOnTextChanged, OnTextChanged)

		SLATE_EVENT(FOnExecuted, OnExecuted)
	SLATE_END_ARGS()

	void Construct( const FArguments& InArgs );

private:
	virtual FReply OnKeyChar(const FGeometry& MyGeometry,const FCharacterEvent& InCharacterEvent) override;
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	FOnExecuted OnExecuted;
};