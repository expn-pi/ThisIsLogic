#pragma once

#include "CoreMinimal.h"

enum class ETokenKind : uint8
{
	Proposition,
	TrueConstant,
	FalseConstant,
	Not,
	And,
	Or,
	Xor,
	Nand,
	Nor,
	Implies,
	ImpliedBy,
	Iff,
	OpenParenthesis,
	CloseParenthesis
};

struct FToken
{
	FToken(
		ETokenKind InKind,
		const FString& InText
	)
	{
		this->Kind = InKind;
		this->Text = InText;

		bool bIsProposition =
			this->Kind == ETokenKind::Proposition;

		if (bIsProposition)
		{
			const TCHAR* NumberText = *this->Text;

			this->PropositionNumber =
				FCString::Atoi(NumberText);
		}
	}

	ETokenKind Kind;

	FString Text;

	int32 PropositionNumber = 0;
};