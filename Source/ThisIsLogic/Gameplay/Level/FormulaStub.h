#pragma once

#include "CoreMinimal.h"

#include "../../Logic/LogicTypes.h"

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

			TArray<int32> Indices = {
				0,
				-1,
				-1,
				1,
				-1,
				2,
				-1,
				3,
				-1,
				-1,
				-1,
				1,
				-1,
				2,
				-1,
				-1,
				3,
				-1,
				-1,
				-1,
				1,
				-1,
				-1,
				2,
				-1
			};

			int32 KindsNum = Kinds.Num();
			int32 IndicesNum = Indices.Num();

			bool bHasSameCount = KindsNum == IndicesNum;

			check(bHasSameCount);

			TArray<FToken> Tokens;

			for (int32 I=0; I<KindsNum; I++)
			{
				ETokenKind Kind = Kinds[I];

				int32 Index = Indices[I];

				FToken Token = {
					.Kind = Kind,
					.Index = Index
				};

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