#pragma once

#include <map>
#include <vector>
#include <cstddef>
#include <optional>

#include "Expression.h"
#include "LogicTypes.h"

#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"

class FFormulaParser
{
	public:

		static std::optional<FExpression>
			Parse(const std::vector<FToken>& Tokens)
		{
			FFormulaParser Parser = FFormulaParser(Tokens);

			return Parser.ParseFormula();
		}
	
	#pragma region PrivateMethods
	private:

		explicit FFormulaParser(
			const std::vector<FToken>& InTokens
		) :
			Tokens(InTokens)
		{
		}

		std::optional<FExpression> ParseFormula()
		{
			bool bIsDebug = false;

			std::optional<FExpression> Formula =
				this->ParseExpression(0);

			bool bIsAtEnd = this->IsAtEnd();

			if (bIsDebug)
			{
				std::size_t TokenCount = this->Tokens.size();

				bool bIsParsed = Formula.has_value();

				UE_LOGFMT(
					LogTemp, Warning,
					"ParseFormula: Tokens {0},"
					" Parsed {1}, End {2}",
					TokenCount, bIsParsed, bIsAtEnd
				);
			}

			if (bIsAtEnd)
			{
				return Formula;
			}
			else
			{
				return std::nullopt;
			}
		}

		std::optional<FExpression>
			ParseExpression(int MinPrecedence)
		{
			bool bIsDebug = false;

			if (bIsDebug)
			{
				UE_LOGFMT(
					LogTemp, Warning,
					"ParseExpression start:"
					" Min {0}, Position {1}",
					MinPrecedence, this->Position
				);
			}

			std::optional<FExpression> Expression =
				this->ParseOperand();

			bool bContinues =
				this->Continues(Expression, MinPrecedence);

			while (bContinues)
			{
				FExpression Left = Expression.value();

				Expression = this->ParseOperation(Left);

				bContinues =
					this->Continues(Expression, MinPrecedence);
			}

			if (bIsDebug)
			{
				UE_LOGFMT(
					LogTemp, Warning,
					"ParseExpression end:"
					" Min {0}, Position {1}",
					MinPrecedence, this->Position
				);
			}

			return Expression;
		}
	
		bool Continues(
			const std::optional<FExpression>& Expression,
			int MinPrecedence
		) const
		{
			bool bIsValid = Expression.has_value();

			bool bBinds = this->NextBinds(MinPrecedence);

			return bIsValid && bBinds;
		}

		bool NextBinds(int MinPrecedence) const
		{
			std::optional<ETokenKind> NextKind =
				this->PeekKind();

			bool bHasNext = NextKind.has_value();

			if (bHasNext)
			{
				ETokenKind Kind = NextKind.value();

				return
					FFormulaParser::Binds(Kind, MinPrecedence);
			}
			else
			{
				return false;
			}
		}

		static bool Binds(
			ETokenKind Kind, int MinPrecedence
		)
		{
			bool bIsOperator =
				FFormulaParser::Rules.contains(Kind);

			if (bIsOperator)
			{
				const FOperatorRule& Rule =
					FFormulaParser::Rules.at(Kind);

				return Rule.Precedence >= MinPrecedence;
			}
			else
			{
				return false;
			}
		}
	
		std::optional<FExpression>
			ParseOperation(const FExpression& Left)
		{
			bool bIsDebug = false;

			FToken Operator = this->TakeToken();

			const FOperatorRule& Rule =
				FFormulaParser::Rules.at(Operator.Kind);

			if (bIsDebug)
			{
				int KindIndex =
					static_cast<int>(Operator.Kind);

				UE_LOGFMT(
					LogTemp, Warning,
					"ParseOperation: Kind {0}, Precedence {1}",
					KindIndex, Rule.Precedence
				);
			}

			bool bIsFlat = Rule.Grouping == EGrouping::Flat;

			if (bIsFlat)
			{
				return this->ParseFlat(
					Operator, Left, Rule.Precedence
				);
			}
			else
			{
				return this->ParseRight(
					Operator, Left, Rule.Precedence
				);
			}
		}

		std::optional<FExpression> ParseFlat(
			const FToken& Operator, const FExpression& First,
			int Precedence
		)
		{
			std::vector<FExpression> Operands = { First };

			int OperandPrecedence = Precedence + 1;

			bool bContinues = true;

			while (bContinues)
			{
				std::optional<FExpression> Operand =
					this->ParseExpression(OperandPrecedence);

				bool bIsValid = Operand.has_value();

				if (bIsValid)
				{
					FExpression ValidOperand = Operand.value();

					Operands.push_back(ValidOperand);
				}
				else
				{
					return std::nullopt;
				}

				bContinues = this->TakeIfNext(Operator.Kind);
			}

			FExpression Expression =
				FExpression(Operator, Operands);

			return Expression;
		}

		std::optional<FExpression> ParseRight(
			const FToken& Operator, const FExpression& Left,
			int Precedence
		)
		{
			std::optional<FExpression> Right =
				this->ParseExpression(Precedence);

			bool bIsValid = Right.has_value();

			if (bIsValid)
			{
				FExpression ValidRight = Right.value();

				std::vector<FExpression> Operands = {
					Left, ValidRight
				};

				FExpression Expression =
					FExpression(Operator, Operands);

				return Expression;
			}
			else
			{
				return std::nullopt;
			}
		}
	
		std::optional<FExpression> ParseOperand()
		{
			bool bIsDebug = false;

			bool bIsAtEnd = this->IsAtEnd();

			if (bIsAtEnd)
			{
				return std::nullopt;
			}
			else
			{
				FToken Token = this->TakeToken();

				if (bIsDebug)
				{
					int KindIndex =
						static_cast<int>(Token.Kind);

					UE_LOGFMT(
						LogTemp, Warning,
						"ParseOperand: Kind {0}, Index {1}",
						KindIndex, Token.Index
					);
				}

				return this->ParseOperandFrom(Token);
			}
		}

		std::optional<FExpression>
			ParseOperandFrom(const FToken& Token)
		{
			switch (Token.Kind)
			{
				case ETokenKind::Proposition:
				case ETokenKind::TrueConstant:
				case ETokenKind::FalseConstant:

					return FExpression(Token);

				case ETokenKind::Not:

					return this->ParseNegation(Token);

				case ETokenKind::OpenParenthesis:

					return this->ParseGroup();

				default:

					return std::nullopt;
			}
		}

		std::optional<FExpression>
			ParseNegation(const FToken& Operator)
		{
			std::optional<FExpression> Operand =
				this->ParseOperand();

			bool bIsValid = Operand.has_value();

			if (bIsValid)
			{
				FExpression ValidOperand = Operand.value();

				std::vector<FExpression> Operands = {
					ValidOperand
				};

				FExpression Negation =
					FExpression(Operator, Operands);

				return Negation;
			}
			else
			{
				return std::nullopt;
			}
		}

		std::optional<FExpression> ParseGroup()
		{
			std::optional<FExpression> Inner =
				this->ParseExpression(0);

			bool bIsValid = Inner.has_value();

			bool bIsClosed =
				this->TakeIfNext(ETokenKind::CloseParenthesis);

			if (bIsValid && bIsClosed)
			{
				return Inner;
			}
			else
			{
				return std::nullopt;
			}
		}

		bool IsAtEnd() const
		{
			std::size_t Count = this->Tokens.size();

			return this->Position == Count;
		}

		std::optional<ETokenKind> PeekKind() const
		{
			bool bIsAtEnd = this->IsAtEnd();

			if (bIsAtEnd)
			{
				return std::nullopt;
			}
			else
			{
				const FToken& Next = this->Tokens[this->Position];

				return Next.Kind;
			}
		}

		FToken TakeToken()
		{
			size_t ActualPosition = this->Position;
			
			this->Position++;

			return this->Tokens[ActualPosition];
		}

		bool TakeIfNext(ETokenKind Kind)
		{
			std::optional<ETokenKind> NextKind =
				this->PeekKind();

			bool bIsNext = NextKind == Kind;

			if (bIsNext)
			{
				this->Position++;
			}

			return bIsNext;
		}
	
	#pragma endregion
	
		std::vector<FToken> Tokens;

		std::size_t Position = 0;

		static inline const std::map<ETokenKind, FOperatorRule>
			Rules = 
			{
			{
					ETokenKind::And,
				{ 7, EGrouping::Flat}
				},
			{
					ETokenKind::Nand,
				{6, EGrouping::Flat}
				},
			{
					ETokenKind::Xor,
				{ 5, EGrouping::Flat}
				},
			{
					ETokenKind::Or,
				{ 4, EGrouping::Flat } 
				},
			{
					ETokenKind::Nor,
				{ 3, EGrouping::Flat } },
			{
					ETokenKind::Implies,
				{ 2, EGrouping::Right } 
				},
			{ 
					ETokenKind::ImpliedBy,
				{ 2, EGrouping::Right }
				},
			{
					ETokenKind::Iff,
				{ 1, EGrouping::Right }
				}
			};
};