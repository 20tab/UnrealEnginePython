// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Runtime/Slate/Public/Framework/Text/SyntaxTokenizer.h"

/**
 * Tokenize the text based upon the given rule set
 */
class FPythonSyntaxTokenizer
{
public:

	/** Denotes the type of token */
	enum class ETokenType : uint8
	{
		/** Syntax token matching the tokenizing rules provided */
		Syntax,
		/** String token containing some literal text */
		Literal,
	};

	enum class ESyntaxType : uint8
	{
		None,
		Keywords,
		Operators,
		Terminals,
		Function,
	};

	/** A token referencing a range in the original text */
	struct FToken
	{
		FToken(const ETokenType InType, const FTextRange& InRange,const ESyntaxType& InSyntaxType)
			: Type(InType)
			, Range(InRange)
			, SyntaxType(InSyntaxType)
		{
		}

		ETokenType Type;
		FTextRange Range;
		ESyntaxType SyntaxType;
	};

	/** A line containing a series of tokens */
	struct FTokenizedLine
	{
		FTextRange Range;
		TArray<FToken> Tokens;
	};

	/** Rule used to match syntax token types */
	struct FRule
	{
		FRule(FString InMatchText, const ESyntaxType& InSyntaxType)
			: MatchText(MoveTemp(InMatchText))
			, SyntaxType(InSyntaxType)
		{
		}

		FString MatchText;
		ESyntaxType SyntaxType;
	};

	/** 
	 * Create a new tokenizer which will use the given rules to match syntax tokens
	 * @param InRules Rules to control the tokenizer, processed in-order so the most greedy matches must come first
	 */
	static TSharedRef< FPythonSyntaxTokenizer > Create(TArray<FRule> InRules);

	virtual ~FPythonSyntaxTokenizer();

	void Process(TArray<FTokenizedLine>& OutTokenizedLines, const FString& Input);

private:

	FPythonSyntaxTokenizer(TArray<FRule> InRules);

	void TokenizeLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges, TArray<FTokenizedLine>& OutTokenizedLines);

	/** Rules to control the tokenizer, processed in-order so the most greedy matches must come first */
	TArray<FRule> Rules;

};
