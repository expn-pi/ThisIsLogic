#pragma once

#include "CoreMinimal.h"

#include "Palette.h"
#include "../../Logic/LogicTypes.h"

#include "OperatorPalette.generated.h"

UCLASS()
class THISISLOGIC_API AOperatorPalette : public APalette
{
	GENERATED_BODY()

	protected:

		virtual TArray<FToken> GetItemTokens() const override
		{
			TArray<ETokenKind> Kinds = {
				ETokenKind::And,
				ETokenKind::Or,
				ETokenKind::Nand,
				ETokenKind::Nor,
				ETokenKind::Implies,
				ETokenKind::ImpliedBy,
				ETokenKind::Iff,
				ETokenKind::Xor,
				ETokenKind::TrueConstant,
				ETokenKind::FalseConstant,
				ETokenKind::OpenParenthesis,
				ETokenKind::CloseParenthesis,
				ETokenKind::Not
			};

			TArray<FToken> Tokens;

			for (ETokenKind Kind : Kinds)
			{
				FToken Token = { .Kind = Kind };

				Tokens.Add(Token);
			}

			return Tokens;
		}

		virtual double GetMaxRowWidth() const override
		{
			double Columns = AOperatorPalette::ColumnCount;

			return AAbstractBlocksManager::BlockHeight * Columns;
		}

	private:

		static constexpr int32 ColumnCount = 1;
};