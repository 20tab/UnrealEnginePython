// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Runtime/SlateCore/Public/Styling/SlateTypes.h"
#include "PythonSyntaxTokenizer.h"
#include "Runtime/Slate/Public/Framework/Text/PlainTextLayoutMarshaller.h"
#include "Runtime/Slate/Public/Framework/Text/ITextDecorator.h"
#include "PythonEditorStyle.h"


class  FPYRichTextSyntaxHighlighterTextLayoutMarshaller : public FPlainTextLayoutMarshaller
{
public:

	struct FSyntaxTextStyle
	{
		FSyntaxTextStyle()
			: NormalTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Normal"))
			, OperatorTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Operator"))
			, KeywordTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Keyword"))
			, StringTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.String"))
			, NumberTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Number"))
			, CommentTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Comment"))
			, BuiltInKeywordTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.BuiltIn"))
			, DefineTextStyle(FPythonEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("SyntaxHighlight.PY.Define"))

		{
		}

		FSyntaxTextStyle(const FTextBlockStyle& InNormalTextStyle, const FTextBlockStyle& InOperatorTextStyle, const FTextBlockStyle& InKeywordTextStyle, const FTextBlockStyle& InStringTextStyle, const FTextBlockStyle& InNumberTextStyle, const FTextBlockStyle& InCommentTextStyle, const FTextBlockStyle& InBuiltInKeywordTextStyle, const FTextBlockStyle& InDefineTextStyle)
			: NormalTextStyle(InNormalTextStyle)
			, OperatorTextStyle(InOperatorTextStyle)
			, KeywordTextStyle(InKeywordTextStyle)
			, StringTextStyle(InStringTextStyle)
			, NumberTextStyle(InNumberTextStyle)
			, CommentTextStyle(InCommentTextStyle)
			, BuiltInKeywordTextStyle(InBuiltInKeywordTextStyle)
			, DefineTextStyle(InDefineTextStyle)
		{
		}

		FTextBlockStyle NormalTextStyle;
		FTextBlockStyle OperatorTextStyle;
		FTextBlockStyle KeywordTextStyle;
		FTextBlockStyle StringTextStyle;
		FTextBlockStyle NumberTextStyle;
		FTextBlockStyle CommentTextStyle;
		FTextBlockStyle BuiltInKeywordTextStyle;
		FTextBlockStyle DefineTextStyle;
	};

	static TSharedRef< FPYRichTextSyntaxHighlighterTextLayoutMarshaller > Create(const FSyntaxTextStyle& InSyntaxTextStyle);

public:

	virtual ~FPYRichTextSyntaxHighlighterTextLayoutMarshaller();

	// ITextLayoutMarshaller
	virtual void SetText(const FString& SourceString, FTextLayout& TargetTextLayout) override;
	virtual bool RequiresLiveUpdate() const override;

	void EnableSyntaxHighlighting(const bool bEnable);
	bool IsSyntaxHighlightingEnabled() const;

protected:

	void ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<FPythonSyntaxTokenizer::FTokenizedLine> TokenizedLines);


	/** Tokenizer used to style the text */
	TSharedPtr< FPythonSyntaxTokenizer > Tokenizer;

	/** True if syntax highlighting is enabled, false to fallback to plain text */
	bool bSyntaxHighlightingEnabled;

	FPYRichTextSyntaxHighlighterTextLayoutMarshaller(TSharedPtr< FPythonSyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle);

	/** Styles used to display the text */
	FSyntaxTextStyle SyntaxTextStyle;

	/** String representing tabs */
	FString TabString;

};
