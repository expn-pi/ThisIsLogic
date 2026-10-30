#pragma once

#include <vector>
#include <cstddef>

#include "LogicTypes.h"

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

class FExpression
{
	#pragma region PublicMethods
	public:

		explicit FExpression(const FToken& InToken) :
			Token(InToken)
		{
		}

		FExpression(
			const FToken& InToken,
			const std::vector<FExpression>& InOperands
		) :
			Token(InToken),
			Operands(InOperands)
		{
		}

		bool operator ==
			(const FExpression& Other) const = default;

		bool Evaluate(const std::vector<bool>& Values) const
		{
			switch (this->Token.Kind)
			{
				case ETokenKind::Proposition:

					return Values[this->Token.Index];

				case ETokenKind::TrueConstant:

					return true;

				case ETokenKind::FalseConstant:

					return false;

				case ETokenKind::Implies:
				case ETokenKind::ImpliedBy:

					return this->EvaluateImplication(Values);

				default:

					return this->EvaluateByCount(Values);
			}
		}

		bool IsEquivalentTo(const FExpression& Other) const
		{
			int ValueCount = this->GetValueCount();

			int OtherValueCount = Other.GetValueCount();

			bool bOtherNeedsMore = OtherValueCount > ValueCount;

			if (bOtherNeedsMore)
			{
				ValueCount = OtherValueCount;
			}

			return this->IsRowsEquivalentTo(ValueCount, Other);
		}

		bool HasSameSizeAs(const FExpression& Other) const
		{
			bool bIsDebug = false;

			int LeafCount = this->CountLeaves();

			int OtherLeafCount = Other.CountLeaves();

			if (bIsDebug)
			{
				UE_LOGFMT(
					LogTemp, Warning,
					"Leaves: this {0}, other {1}",
					LeafCount, OtherLeafCount
				);
			}

			return LeafCount == OtherLeafCount;
		}
	
	#pragma endregion
	
	#pragma region PrivateMethods
	private:

		bool EvaluateImplication(const std::vector<bool>& Values)
			const
		{
			const FExpression& First = this->Operands[0];

			const FExpression& Second = this->Operands[1];

			bool bFirst = First.Evaluate(Values);

			bool bSecond = Second.Evaluate(Values);

			bool bIsForward =
				this->Token.Kind == ETokenKind::Implies;

			if (bIsForward)
			{
				return !bFirst || bSecond;
			}
			else
			{
				return bFirst || !bSecond;
			}
		}

		bool EvaluateByCount(const std::vector<bool>& Values)
			const
		{
			std::size_t TrueCount = this->CountTrueOperands(Values);

			std::size_t OperandCount = this->Operands.size();

			switch (this->Token.Kind)
			{
				case ETokenKind::Not:
				case ETokenKind::Nor:

					return TrueCount == 0;

				case ETokenKind::And:

					return TrueCount == OperandCount;

				case ETokenKind::Nand:

					return TrueCount < OperandCount;

				case ETokenKind::Or:

					return TrueCount > 0;

				case ETokenKind::Xor:

					return TrueCount % 2 == 1;

				default:

					return TrueCount != 1;
			}
		}

		std::size_t CountTrueOperands(
			const std::vector<bool>& Values
		) const
		{
			std::size_t TrueCount = 0;

			for (const FExpression& Operand : this->Operands)
			{
				bool bIsTrue = Operand.Evaluate(Values);

				if (bIsTrue)
				{
					TrueCount++;
				}
			}

			return TrueCount;
		}
	
		bool IsRowsEquivalentTo(
			int ValueCount, const FExpression& Other
		) const
		{
			bool bIsDebug = false;
			
			int RowCount = 1 << ValueCount;
				
			for (int Row=0; Row<RowCount; Row++)
			{
				std::vector<bool> Values =
					FExpression::GetRowValues(Row, ValueCount);

				bool bValue = this->Evaluate(Values);

				bool bOtherValue = Other.Evaluate(Values);
				
				if (bIsDebug)
				{
					UE_LOGFMT(
						LogTemp, Warning,
						"Row {0}: this {1}, other {2}",
						Row, bValue, bOtherValue
					);	
				}

				bool bDiffers = bValue != bOtherValue;

				if (bDiffers)
				{
					return false;
				}
			}

			return true;
		}
	
		int GetValueCount() const
		{
			bool bIsProposition =
				this->Token.Kind == ETokenKind::Proposition;

			if (bIsProposition)
			{
				return this->Token.Index + 1;
			}
			else
			{
				return this->GetOperandsValueCount();
			}
		}

		int GetOperandsValueCount() const
		{
			int ValueCount = 0;

			for (const FExpression& Operand : this->Operands)
			{
				int OperandValueCount = Operand.GetValueCount();

				bool bNeedsMore = OperandValueCount > ValueCount;

				if (bNeedsMore)
				{
					ValueCount = OperandValueCount;
				}
			}

			return ValueCount;
		}

		int CountLeaves() const
		{
			bool bIsLeaf = this->Operands.empty();

			if (bIsLeaf)
			{
				return 1;
			}
			else
			{
				return this->CountOperandsLeaves();
			}
		}

		int CountOperandsLeaves() const
		{
			int LeafCount = 0;

			for (const FExpression& Operand : this->Operands)
			{
				int OperandLeafCount = Operand.CountLeaves();

				LeafCount += OperandLeafCount;
			}

			return LeafCount;
		}
	
		static std::vector<bool> GetRowValues(
			int Row, int ValueCount
		)
		{
			std::vector<bool> Values;

			for (int Index=0; Index<ValueCount; Index++)
			{
				int Bit = (Row >> Index) & 1;

				bool bValue = Bit == 1;

				Values.push_back(bValue);
			}

			return Values;
		}
	
	#pragma endregion

		FToken Token;

		std::vector<FExpression> Operands;
};