#pragma once

#include "../../Logic/LogicTypes.h"

struct FBlockState
{
	FToken Token;

	bool bMinimized = false;

	bool operator == (const FBlockState& Other) const = default;
};