#pragma once

#include "CoreMinimal.h"

#include "LogicTypes.h"

class FFormulaStub
{
	public:

		TArray<FToken> GetTokens() const
		{
			TArray<ETokenKind> Kinds = {
				ETokenKind::OpenParenthesis,
				ETokenKind::Proposition,
				ETokenKind::And,
				ETokenKind::Not,
				ETokenKind::Proposition,
				ETokenKind::CloseParenthesis,
				ETokenKind::Implies,
				ETokenKind::Proposition
			};

			TArray<FString> Texts = {
				"(",
				"costume",
				"&",
				"~",
				"loud music",
				")",
				"->",
				"party"
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
};