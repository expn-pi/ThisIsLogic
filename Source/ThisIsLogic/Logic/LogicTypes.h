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
	}

	ETokenKind Kind;

	FString Text;
};