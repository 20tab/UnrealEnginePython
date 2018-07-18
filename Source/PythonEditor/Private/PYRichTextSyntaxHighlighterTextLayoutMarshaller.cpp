// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PYRichTextSyntaxHighlighterTextLayoutMarshaller.h"
#include "WhiteSpaceTextRun.h"
#include "Runtime/Launch/Resources/Version.h"

FPYRichTextSyntaxHighlighterTextLayoutMarshaller::FPYRichTextSyntaxHighlighterTextLayoutMarshaller(TSharedPtr< FPythonSyntaxTokenizer > InTokenizer, const FSyntaxTextStyle& InSyntaxTextStyle)
	: Tokenizer(MoveTemp(InTokenizer))
	, bSyntaxHighlightingEnabled(true)
	, SyntaxTextStyle(InSyntaxTextStyle)
{
}


FPYRichTextSyntaxHighlighterTextLayoutMarshaller::~FPYRichTextSyntaxHighlighterTextLayoutMarshaller()
{
}

void FPYRichTextSyntaxHighlighterTextLayoutMarshaller::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	if (bSyntaxHighlightingEnabled)
	{
		TArray<FPythonSyntaxTokenizer::FTokenizedLine> TokenizedLines;
		Tokenizer->Process(TokenizedLines, SourceString);
		ParseTokens(SourceString, TargetTextLayout, TokenizedLines);
	}
	else
	{
		FPlainTextLayoutMarshaller::SetText(SourceString, TargetTextLayout);
	}
}

bool FPYRichTextSyntaxHighlighterTextLayoutMarshaller::RequiresLiveUpdate() const
{
	return bSyntaxHighlightingEnabled;
}

void FPYRichTextSyntaxHighlighterTextLayoutMarshaller::EnableSyntaxHighlighting(const bool bEnable)
{
	bSyntaxHighlightingEnabled = bEnable;
	MakeDirty();
}

bool FPYRichTextSyntaxHighlighterTextLayoutMarshaller::IsSyntaxHighlightingEnabled() const
{
	return bSyntaxHighlightingEnabled;
}



const TCHAR* Keywords[] =
{
	TEXT("and"),
	TEXT("assert"),
	TEXT("break"),
	TEXT("continue"),
	TEXT("del"),
	TEXT("elif"),
	TEXT("else"),
	TEXT("except"),
	TEXT("exec"),
	TEXT("finally"),
	TEXT("for"),
	TEXT("from"),
	TEXT("globals"),
	TEXT("global"),
	TEXT("if"),
	TEXT("import"),
	TEXT("in"),
	TEXT("is"),
	TEXT("lambda"),
	TEXT("not"),
	TEXT("or"),
	TEXT("pass"),
	TEXT("print"),
	TEXT("raise"),
	TEXT("return"),
	TEXT("try"),
	TEXT("while"),
	TEXT("yield"),
	TEXT("None"),
	TEXT("True"),
	TEXT("False"),
	TEXT("import"),
	TEXT("as")
};

const TCHAR* Operators[] =
{
	TEXT("="),
	TEXT("=="),
	TEXT("!="),
	TEXT("<"),
	TEXT("<="),
	TEXT(">"),
	TEXT(">="),
	TEXT("+"),
	TEXT("-"),
	TEXT("*"),
	TEXT("/"),
	TEXT("//"),
	TEXT("%"),
	TEXT("**"),
	TEXT("+="),
	TEXT("-="),
	TEXT("*="),
	TEXT("/="),
	TEXT("%="),
	TEXT("^"),
	TEXT("|"),
	TEXT("&"),
	TEXT("~"),
	TEXT(">>"),
	TEXT("<<"),
	TEXT("'"),
	TEXT("\"\"\""),
	TEXT("\""),
	TEXT("#"),
};

const TCHAR* Terminals[] =
{
	TEXT("("),
	TEXT(")"),
	TEXT("["),
	TEXT("]"),
	TEXT("{"),
	TEXT("}"),
	TEXT(":"),

};

const TCHAR* BuiltInKeywords[] =
{
	TEXT("def"),
	TEXT("classmethod"),
	TEXT("class"),
	TEXT("abs"),
	TEXT("all"),
	TEXT("any"),
	TEXT("basestring"),
	TEXT("bin"),
	TEXT("bool"),
	TEXT("bytearray"),
	TEXT("callable"),
	TEXT("chr"),
	TEXT("cmp"),
	TEXT("compile"),
	TEXT("complex"),
	TEXT("delattr"),
	TEXT("dict"),
	TEXT("dir"),
	TEXT("divmod"),
	TEXT("enumerate"),
	TEXT("eval"),
	TEXT("execfile"),
	TEXT("file"),
	TEXT("filter"),
	TEXT("float"),
	TEXT("format"),
	TEXT("frozenset"),
	TEXT("getattr"),
	TEXT("hasattr"),
	TEXT("hash"),
	TEXT("help"),
	TEXT("hex"),
	TEXT("id"),
	TEXT("input"),
	TEXT("int"),
	TEXT("isinstance"),
	TEXT("issubclass"),
	TEXT("iter"),
	TEXT("len"),
	TEXT("list"),
	TEXT("locals"),
	TEXT("long"),
	TEXT("map"),
	TEXT("max"),
	TEXT("memoryview"),
	TEXT("memoryview"),
	TEXT("next"),
	TEXT("object"),
	TEXT("oct"),
	TEXT("open"),
	TEXT("ord"),
	TEXT("pow"),
	TEXT("property"),
	TEXT("range"),
	TEXT("raw_input"),
	TEXT("reduce"),
	TEXT("reload"),
	TEXT("repr"),
	TEXT("reversed"),
	TEXT("round"),
	TEXT("set"),
	TEXT("setattr"),
	TEXT("slice"),
	TEXT("sorted"),
	TEXT("staticmethod"),
	TEXT("str"),
	TEXT("sum"),
	TEXT("super"),
	TEXT("tuple"),
	TEXT("type"),
	TEXT("unichr"),
	TEXT("unicode"),
	TEXT("vars"),
	TEXT("xrange"),
	TEXT("zip"),
	TEXT("apply"),
	TEXT("buffer"),
	TEXT("coerce"),
	TEXT("intern"),
	TEXT("__init__"),
	TEXT("__import__"),
	TEXT("__new__"),
	TEXT("__str__"),
	TEXT("__getitem__"),
	TEXT("__len__"),
	TEXT("__cmp__"),
	TEXT("__getattr__"),
	TEXT("__name__"),
	TEXT("__setattr__"),
	TEXT("__delattr__"),
	TEXT("__getattribute__"),
	TEXT("__gt__"),
	TEXT("__lt__"),
	TEXT("__ge__"),
	TEXT("__le__"),
	TEXT("__eq__"),
	TEXT("__call__"),
	TEXT("with"),
	TEXT("async"),
	TEXT("await"),
};



TSharedRef< FPYRichTextSyntaxHighlighterTextLayoutMarshaller > FPYRichTextSyntaxHighlighterTextLayoutMarshaller::Create(const FSyntaxTextStyle& InSyntaxTextStyle)
{
	TArray<FPythonSyntaxTokenizer::FRule> TokenizerRules;
	
	// operators
	for(const auto& Operator : Operators)
	{
		TokenizerRules.Emplace(FPythonSyntaxTokenizer::FRule(Operator, FPythonSyntaxTokenizer::ESyntaxType::Operators));
	}
	// Terminals
	for (const auto& Terminal : Terminals)
	{
		TokenizerRules.Emplace(FPythonSyntaxTokenizer::FRule(Terminal, FPythonSyntaxTokenizer::ESyntaxType::Terminals));
	}

	// keywords
	for(const auto& Keyword : Keywords)
	{
		TokenizerRules.Emplace(FPythonSyntaxTokenizer::FRule(Keyword, FPythonSyntaxTokenizer::ESyntaxType::Keywords));
	}

	// Pre-processor Keywords
	for(const auto& BuiltInKeyword : BuiltInKeywords)
	{
		TokenizerRules.Emplace(FPythonSyntaxTokenizer::FRule(BuiltInKeyword, FPythonSyntaxTokenizer::ESyntaxType::Function));
	}
	return MakeShareable(new FPYRichTextSyntaxHighlighterTextLayoutMarshaller(FPythonSyntaxTokenizer::Create(TokenizerRules), InSyntaxTextStyle));
}



void FPYRichTextSyntaxHighlighterTextLayoutMarshaller::ParseTokens(const FString& SourceString, FTextLayout& TargetTextLayout, TArray<FPythonSyntaxTokenizer::FTokenizedLine> TokenizedLines)
{
	enum class EParseState : uint8
	{
		None,
		LookingForString,
		LookingForCharacter,
		LookingForDefine,
		LookingForSingleLineComment,
		LookingForMultiLineString,
	};

	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(TokenizedLines.Num());

	// Parse the tokens, generating the styled runs for each line
	EParseState ParseState = EParseState::None;
	for(const FPythonSyntaxTokenizer::FTokenizedLine& TokenizedLine : TokenizedLines)
	{
		TSharedRef<FString> ModelString = MakeShareable(new FString());
		TArray< TSharedRef< IRun > > Runs;

		if(ParseState == EParseState::LookingForSingleLineComment)
		{
			ParseState = EParseState::None;
		}

		for(const FPythonSyntaxTokenizer::FToken& Token : TokenizedLine.Tokens)
		{
			const FString TokenText = SourceString.Mid(Token.Range.BeginIndex, Token.Range.Len());
			bool hasNextChar = false;
			if (Token.Range.BeginIndex + Token.Range.Len() <= SourceString.Len()-1) {
				const FString NextText = SourceString.Mid(Token.Range.BeginIndex + Token.Range.Len(), 1);
				if (TChar<WIDECHAR>::IsAlpha(NextText[0]) || NextText[0]==TEXT('_')) {
					hasNextChar = true;
				}
			}
			
			const FTextRange ModelRange(ModelString->Len(), ModelString->Len() + TokenText.Len());
			ModelString->Append(TokenText);

			FRunInfo RunInfo(TEXT("SyntaxHighlight.PY.Normal"));
			FTextBlockStyle TextBlockStyle = SyntaxTextStyle.NormalTextStyle;

#if ENGINE_MINOR_VERSION >= 18
			const bool bIsWhitespace = FString(TokenText).TrimEnd().IsEmpty();
#else
			const bool bIsWhitespace = FString(TokenText).TrimTrailing().IsEmpty();
#endif
			if(!bIsWhitespace)
			{
				bool bHasMatchedSyntax = false;
				if(Token.Type == FPythonSyntaxTokenizer::ETokenType::Syntax)
				{
					if (ParseState == EParseState::None && TokenText == TEXT("\"\"\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForMultiLineString;
					}
					else if (ParseState == EParseState::LookingForMultiLineString && TokenText == TEXT("\"\"\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && TokenText == TEXT("\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForString;
						bHasMatchedSyntax = true;
					}
					else if(ParseState == EParseState::LookingForString && TokenText == TEXT("\""))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Normal");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::None;
					}
					else if(ParseState == EParseState::None && TokenText == TEXT("\'"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::LookingForCharacter;
						bHasMatchedSyntax = true;
					}
					else if(ParseState == EParseState::LookingForCharacter && TokenText == TEXT("\'"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Normal");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
						ParseState = EParseState::None;
					}
					//else if(ParseState == EParseState::None && TokenText.StartsWith(TEXT("#")))
					//{
					//	RunInfo.Name = TEXT("SyntaxHighlight.CPP.PreProcessorKeyword");
					//	TextBlockStyle = SyntaxTextStyle.PreProcessorKeywordTextStyle;
					//	ParseState = EParseState::None;
					//}
					else if(ParseState == EParseState::None && TokenText == TEXT("#"))
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
						ParseState = EParseState::LookingForSingleLineComment;
					}

					else if(ParseState == EParseState::None && !hasNextChar && Token.SyntaxType==FPythonSyntaxTokenizer::ESyntaxType::Keywords)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Keyword");
						TextBlockStyle = SyntaxTextStyle.KeywordTextStyle;
						ParseState = EParseState::None;
					}
					else if (ParseState == EParseState::None && !hasNextChar && Token.SyntaxType == FPythonSyntaxTokenizer::ESyntaxType::Function)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.BuiltIn");
						TextBlockStyle = SyntaxTextStyle.BuiltInKeywordTextStyle;
						ParseState = EParseState::None;
						if (TokenText == TEXT("def") || TokenText == TEXT("class")) {
							bHasMatchedSyntax = true;
							ParseState = EParseState::LookingForDefine;
						}
					}
					else if (ParseState == EParseState::LookingForDefine && Token.SyntaxType == FPythonSyntaxTokenizer::ESyntaxType::Terminals)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Normal");
						TextBlockStyle = SyntaxTextStyle.NormalTextStyle;
						ParseState = EParseState::None;
					}
					else if (ParseState == EParseState::None && Token.SyntaxType == FPythonSyntaxTokenizer::ESyntaxType::Operators)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Operator");
						TextBlockStyle = SyntaxTextStyle.OperatorTextStyle;
						ParseState = EParseState::None;
					}

				}
				
				// It's possible that we fail to match a syntax token if we're in a state where it isn't parsed
				// In this case, we treat it as a literal token
				if(Token.Type == FPythonSyntaxTokenizer::ETokenType::Literal || !bHasMatchedSyntax)
				{
					if(ParseState == EParseState::LookingForString)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
					}
					else if (ParseState == EParseState::LookingForDefine)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Define");
						TextBlockStyle = SyntaxTextStyle.DefineTextStyle;
					}
					else if(ParseState == EParseState::LookingForCharacter)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
					}
					else if(ParseState == EParseState::LookingForSingleLineComment)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.Comment");
						TextBlockStyle = SyntaxTextStyle.CommentTextStyle;
					}
					else if(ParseState == EParseState::LookingForMultiLineString)
					{
						RunInfo.Name = TEXT("SyntaxHighlight.PY.String");
						TextBlockStyle = SyntaxTextStyle.StringTextStyle;
					}
				}

				TSharedRef< ISlateRun > Run = FSlateTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange);
				Runs.Add(Run);
			}
			else
			{
				RunInfo.Name = TEXT("SyntaxHighlight.CPP.WhiteSpace");
				TSharedRef< ISlateRun > Run = FWhiteSpaceTextRun::Create(RunInfo, ModelString, TextBlockStyle, ModelRange, 4);
				Runs.Add(Run);
			}
		}

		LinesToAdd.Emplace(MoveTemp(ModelString), MoveTemp(Runs));
	}

	TargetTextLayout.AddLines(LinesToAdd);
}


