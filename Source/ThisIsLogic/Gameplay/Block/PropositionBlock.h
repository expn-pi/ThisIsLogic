#pragma once

#include "Block.h"
#include "CoreMinimal.h"
#include "PropositionBlock.generated.h"

UCLASS()
class THISISLOGIC_API APropositionBlock : public ABlock
{
	GENERATED_BODY()
	
	#pragma region PublicMethods
	public:
	
		APropositionBlock()
		{
			FLinearColor BlockColor =
				FLinearColor(0.9f, 0.3f, 0.1f);

			this->SetColor(BlockColor);
		}

		void SetProposition(
			const FString& NewLetter, const FString& NewSentence
		)
		{
			this->Letter = NewLetter;
			
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

		virtual FBlockState GetState() const override
		{
			FBlockState State = Super::GetState();

			State.bMinimized = this->bMinimized;

			return State;
		}

		virtual void ApplyState(const FBlockState& State)
			override
		{
			Super::ApplyState(State);
			
			bool bStateMinimized = State.bMinimized;
			
			this->SetMinimized(bStateMinimized);
		}
	
	#pragma endregion
	
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

		FString Letter;

		bool bMinimized = false;
};