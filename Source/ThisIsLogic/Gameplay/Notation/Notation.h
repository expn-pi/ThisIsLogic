#pragma once

#include "CoreMinimal.h"

#include "../../Logic/LogicTypes.h"

class FNotation
{
	public:

		explicit FNotation(TCHAR InFirstLetter)
		{
			this->FirstLetter = InFirstLetter;

			this->FillSymbols();
		}

		FString GetLetter(int32 PropositionIndex) const
		{
			int32 LetterCode =
				this->FirstLetter + PropositionIndex;

			TCHAR Letter = static_cast<TCHAR>(LetterCode);

			bool bIsLetter = FChar::IsAlpha(Letter);

			check(bIsLetter);

			return FString::Chr(Letter);
		}

		FString GetSymbol(ETokenKind Kind) const
		{
			return this->Symbols[Kind];
		}

	private:

		void FillSymbols()
		{
			TArray<ETokenKind> Kinds = {
				ETokenKind::TrueConstant,
				ETokenKind::FalseConstant,
				ETokenKind::Not,
				ETokenKind::And,
				ETokenKind::Or,
				ETokenKind::Xor,
				ETokenKind::Nand,
				ETokenKind::Nor,
				ETokenKind::Implies,
				ETokenKind::ImpliedBy,
				ETokenKind::Iff,
				ETokenKind::OpenParenthesis,
				ETokenKind::CloseParenthesis
			};

			TArray<FString> Texts = {
				"1",
				"0",
				"~",
				"&",
				"|",
				"XOR",
				"NAND",
				"NOR",
				"->",
				"<-",
				"=",
				"(",
				")"
			};

			int32 KindsNum = Kinds.Num();
			int32 TextsNum = Texts.Num();

			bool bHasSameCount = KindsNum == TextsNum;

			check(bHasSameCount);

			for (int32 Index=0; Index<KindsNum; Index++)
			{
				ETokenKind Kind = Kinds[Index];

				const FString& Text = Texts[Index];

				this->Symbols.Add(Kind, Text);
			}
		}

		TCHAR FirstLetter;

		TMap<ETokenKind, FString> Symbols;
};