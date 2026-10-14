#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include "LogicTypes.h"

class FFormulaAnalyzer
{
	public:
	
		FFormulaAnalyzer() = delete;
	
		static std::optional<std::vector<FToken>>
			Analyze(const std::string& Formula)
		{
			std::istringstream Stream =
				std::istringstream(Formula);

			std::vector<FToken> Tokens;

			std::string Piece;

			while (Stream >> Piece)
			{
				std::optional<FToken> Token =
					FFormulaAnalyzer::AnalyzePiece(Piece);

				bool bIsKnown = Token.has_value();

				if (bIsKnown)
				{
					FToken KnownToken = Token.value();

					Tokens.push_back(KnownToken);
				}
				else
				{
					return std::nullopt;
				}
			}

			return Tokens;
		}
	
	private:
	
		static std::optional<FToken>
			AnalyzePiece(const std::string& Piece)
		{
			bool bIsOperator =
				FFormulaAnalyzer::
					OperatorKinds.contains(Piece);

			if (bIsOperator)
			{
				ETokenKind Kind =
					FFormulaAnalyzer::
						OperatorKinds.at(Piece);

				FToken Token = {
					.Kind = Kind
				};

				return Token;
			}
			else
			{
				return
					FFormulaAnalyzer::
						AnalyzeProposition(Piece);
			}
		}
	
		static std::optional<FToken>
			AnalyzeProposition(const std::string& Piece)
		{
			std::size_t FirstNonDigit =
				Piece.find_first_not_of("0123456789");

			bool bIsProposition =
				FirstNonDigit == std::string::npos;
				
			if (bIsProposition)
			{
				int Number = std::stoi(Piece);

				int Index = Number - 1;

				FToken Token = {
					.Kind = ETokenKind::Proposition,
					.Index = Index
				};

				return Token;
			}
			else
			{
				return std::nullopt;
			}
		}
		
		static inline const std::map<std::string, ETokenKind>
			OperatorKinds = {
				{ "TRUE", ETokenKind::TrueConstant },
				{ "FALSE", ETokenKind::FalseConstant },
				{ "NOT", ETokenKind::Not },
				{ "AND", ETokenKind::And },
				{ "OR", ETokenKind::Or },
				{ "XOR", ETokenKind::Xor },
				{ "NAND", ETokenKind::Nand },
				{ "NOR", ETokenKind::Nor },
				{ "THEN", ETokenKind::Implies },
				{ "IF", ETokenKind::ImpliedBy },
				{ "=", ETokenKind::Iff },
				{ "(", ETokenKind::OpenParenthesis },
				{ ")", ETokenKind::CloseParenthesis }
			};
};