#pragma once

#include <cstdint>

enum class ETokenKind : std::uint8_t
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
	ETokenKind Kind;

	int Index = -1;
	
	bool operator==(const FToken& Other) const = default;
};