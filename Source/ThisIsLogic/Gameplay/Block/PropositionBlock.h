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
			FColor FBlockColor = FColor(0xFFFFFF02);
			
			FLinearColor BlockColor = FLinearColor(FBlockColor);

			this->SetColor(BlockColor);
		}

		void SetProposition(
			const FString& NewLetter, const FString& NewSentence
		)
		{
			FString Separator = ": ";

			this->Letter = NewLetter;

			this->MaximizedText =
				NewLetter + Separator + NewSentence;
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
				return this->MaximizedText;
			}
		}
	
		virtual float GetLabelSize() const override
		{
			return
				this->Height *
				APropositionBlock::LabelSizeRatio;
		}
	
		static constexpr float LabelSizeRatio = 0.5f;

	private:

		FString Letter;

		FString MaximizedText;

		bool bMinimized = false;
};