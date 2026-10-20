#pragma once

#include "CoreMinimal.h"

#include "../../Logic/LogicTypes.h"

enum class ESymbolStyle
{
	Ascii,
	Textbook
};

class FNotation
{
	public:

		FNotation() :
			FNotation(
				FNotation::DefaultFirstLetter,
				FNotation::DefaultSymbolStyle
			)
		{
		}

		FNotation(
			TCHAR InFirstLetter,
			ESymbolStyle SymbolStyle
		)
		{
			this->FirstLetter = InFirstLetter;

			this->FillSymbols(SymbolStyle);
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

		void FillSymbols(ESymbolStyle SymbolStyle)
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

			TArray<FString> Texts =
				FNotation::GetTexts(SymbolStyle);

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

		static TArray<FString> GetTexts(
			ESymbolStyle SymbolStyle
		)
		{
			bool bIsAscii =
				SymbolStyle == ESymbolStyle::Ascii;

			if (bIsAscii)
			{
				return FNotation::GetAsciiTexts();
			}
			else
			{
				return FNotation::GetTextbookTexts();
			}
		}

		static TArray<FString> GetAsciiTexts()
		{
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

			return Texts;
		}

		static TArray<FString> GetTextbookTexts()
		{
			TArray<TCHAR> CharacterCodes = {
				0x22A4,
				0x22A5,
				0x00AC,
				0x2227,
				0x2228,
				0x2295,
				0x2191,
				0x2193,
				0x2192,
				0x2190,
				0x2194,
				0x0028,
				0x0029
			};

			TArray<FString> Texts;

			for (TCHAR Code : CharacterCodes)
			{
				FString Text = FString::Chr(Code);

				Texts.Add(Text);
			}

			return Texts;
		}
	
		TCHAR FirstLetter;

		TMap<ETokenKind, FString> Symbols;

		static constexpr TCHAR DefaultFirstLetter = 'P';

		static constexpr ESymbolStyle DefaultSymbolStyle =
			ESymbolStyle::Textbook;
};
