#pragma once

#include "CoreMinimal.h"

#include "Palette.h"
#include "../../Logic/LogicTypes.h"

#include "PropositionPalette.generated.h"

UCLASS()
class THISISLOGIC_API APropositionPalette : public APalette
{
	GENERATED_BODY()

	public:

		void SetSentences(const TArray<FString>& Sentences)
		{
			this->Board->SetSentences(Sentences);

			this->PropositionCount = Sentences.Num();
		}

	protected:

		virtual TArray<FToken> GetItemTokens() const override
		{
			int32 Count = this->PropositionCount;

			TArray<FToken> Tokens;

			for (int32 Index=0; Index<Count; Index++)
			{
				FToken Token = {
					.Kind = ETokenKind::Proposition,
					.Index = Index
				};

				Tokens.Add(Token);
			}

			return Tokens;
		}

		virtual double GetMaxRowWidth() const override
		{
			double Ratio = APropositionPalette::MaxRowWidthRatio;

			return AAbstractBlocksManager::BlockHeight * Ratio;
		}

	private:

		int32 PropositionCount = 0;

		static constexpr double MaxRowWidthRatio = 20.0;
};