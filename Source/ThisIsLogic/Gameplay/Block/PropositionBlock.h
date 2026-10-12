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
	
		void SetMinimized(bool bNewMinimized)
		{
			this->bMinimized = bNewMinimized;

			this->FitToText();
		}
	
		void ToggleMinimized()
		{
			bool bNewMinimized = !this->bMinimized;

			this->SetMinimized(bNewMinimized);
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
	
		FString Sentence;

		FString Letter = TEXT("P");

		bool bMinimized = false;
};