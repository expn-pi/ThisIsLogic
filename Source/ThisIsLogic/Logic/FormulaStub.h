#pragma once

#include "CoreMinimal.h"

#include "LogicTypes.h"

class FFormulaStub
{
	public:

		TArray<FToken> GetTokens() const
		{
			TArray<ETokenKind> Kinds = {
				ETokenKind::Proposition,
				ETokenKind::Iff,
				ETokenKind::OpenParenthesis,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Proposition,
				ETokenKind::CloseParenthesis,
				ETokenKind::Or,
				ETokenKind::OpenParenthesis,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Not,
				ETokenKind::Proposition,
				ETokenKind::CloseParenthesis,
				ETokenKind::Or,
				ETokenKind::OpenParenthesis,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Not,
				ETokenKind::Proposition,
				ETokenKind::CloseParenthesis
			};

			TArray<FString> Texts = {
				"1",
				"=",
				"(",
				"2",
				"&",
				"3",
				"&",
				"4",
				")",
				"|",
				"(",
				"2",
				"&",
				"3",
				"&",
				"~",
				"4",
				")",
				"|",
				"(",
				"2",
				"&",
				"~",
				"3",
				")"
			};

			int32 KindsNum = Kinds.Num();
			int32 TextsNum = Texts.Num();

			bool bHasSameCount = KindsNum == TextsNum;

			check(bHasSameCount);

			TArray<FToken> Tokens;

			for (int32 Index=0; Index<KindsNum; Index++)
			{
				ETokenKind Kind = Kinds[Index];

				const FString& Text = Texts[Index];

				FToken Token = FToken(Kind, Text);

				Tokens.Add(Token);
			}

			return Tokens;
		}

		TArray<FString> GetSentences() const
		{
			TArray<FString> Sentences = {
				"Alarm on",
				"Motion sensor on",
				"Window sensor on",
				"Door sensor on"
			};

			return Sentences;
		}
};