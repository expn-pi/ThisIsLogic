#pragma once

#include "CoreMinimal.h"

#include "Block.h"

#include "PropositionBlock.generated.h"

UCLASS()
class THISISLOGIC_API APropositionBlock :
	public ABlock
{
	GENERATED_BODY()

	public:

		APropositionBlock()
		{
			FLinearColor BlockColor =
				FLinearColor(0.9f, 0.3f, 0.1f);

			this->SetColor(BlockColor);
		}

		void SetSentence(const FString& NewSentence)
		{
			this->Sentence = NewSentence;
		}

		virtual void Tapped() override
		{
			Super::Tapped();

			this->ToggleMinimized();
		}

	protected:

		virtual FString GetText() const override
		{
			if (this->bMinimized)
			{
				return this->Letter;
			}
			else
			{
				return this->Sentence;
			}
		}

	private:

		void ToggleMinimized()
		{
			this->bMinimized = !this->bMinimized;

			this->FitToText();
		}

		FString Sentence;

		FString Letter = TEXT("P");

		bool bMinimized = false;
};