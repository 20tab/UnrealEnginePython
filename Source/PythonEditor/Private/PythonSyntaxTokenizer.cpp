// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "PythonSyntaxTokenizer.h"
#include "Runtime/Core/Public/Internationalization/BreakIterator.h"

TSharedRef< FPythonSyntaxTokenizer > FPythonSyntaxTokenizer::Create(TArray<FRule> InRules)
{
	return MakeShareable(new FPythonSyntaxTokenizer(MoveTemp(InRules)));
}

FPythonSyntaxTokenizer::~FPythonSyntaxTokenizer()
{
}

void FPythonSyntaxTokenizer::Process(TArray<FTokenizedLine>& OutTokenizedLines, const FString& Input)
{
#if UE_ENABLE_ICU
	TArray<FTextRange> LineRanges;
	FTextRange::CalculateLineRangesFromString(Input, LineRanges);
	TokenizeLineRanges(Input, LineRanges, OutTokenizedLines);
#else
	FTokenizedLine FakeTokenizedLine;
	FakeTokenizedLine.Range = FTextRange(0, Input.Len());
	FakeTokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, FakeTokenizedLine.Range));
	OutTokenizedLines.Add(FakeTokenizedLine);
#endif
}

FPythonSyntaxTokenizer::FPythonSyntaxTokenizer(TArray<FRule> InRules)
	: Rules(MoveTemp(InRules))
{
}

void FPythonSyntaxTokenizer::TokenizeLineRanges(const FString& Input, const TArray<FTextRange>& LineRanges, TArray<FTokenizedLine>& OutTokenizedLines)
{
	TSharedRef<IBreakIterator> WBI = FBreakIterator::CreateWordBreakIterator();
	WBI->SetString(Input);

	// Tokenize line ranges
	for(const FTextRange& LineRange : LineRanges)
	{
		FTokenizedLine TokenizedLine;
		TokenizedLine.Range = LineRange;

		if(TokenizedLine.Range.IsEmpty())
		{
			TokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, TokenizedLine.Range, ESyntaxType::None));
		}
		else
		{
			int32 CurrentOffset = LineRange.BeginIndex;
			while(CurrentOffset < LineRange.EndIndex)
			{
				// First check for a match against any syntax token rules
				bool bHasMatchedSyntax = false;
				for(const FRule& Rule : Rules)
				{
					if(FCString::Strncmp(&Input[CurrentOffset], *Rule.MatchText, Rule.MatchText.Len()) == 0)
					{
						const int32 SyntaxTokenEnd = CurrentOffset + Rule.MatchText.Len();
						TokenizedLine.Tokens.Emplace(FToken(ETokenType::Syntax, FTextRange(CurrentOffset, SyntaxTokenEnd), Rule.SyntaxType));

						check(SyntaxTokenEnd <= LineRange.EndIndex);

						bHasMatchedSyntax = true;
						CurrentOffset = SyntaxTokenEnd;
						break;
					}
				}

				if(bHasMatchedSyntax)
				{
					continue;
				}

				// If none matched, consume the character(s) as text
				const int32 NextWordBoundary = WBI->MoveToCandidateAfter(CurrentOffset);
				const int32 TextTokenEnd = (NextWordBoundary == INDEX_NONE) ? LineRange.EndIndex : FMath::Min(NextWordBoundary, LineRange.EndIndex);
				TokenizedLine.Tokens.Emplace(FToken(ETokenType::Literal, FTextRange(CurrentOffset, TextTokenEnd), ESyntaxType::None));
				CurrentOffset = TextTokenEnd;
			}
		}

		OutTokenizedLines.Add(TokenizedLine);
	}
}
