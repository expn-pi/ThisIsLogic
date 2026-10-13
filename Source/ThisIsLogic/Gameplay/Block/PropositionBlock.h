#pragma once

#include "Block.h"
#include "CoreMinimal.h"
#include "PropositionBlock.generated.h"

class APropositionBlock;

DECLARE_DELEGATE_OneParam(
	FOnPropositionToggleMinimizedRequested,
	APropositionBlock*
);

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

		void SetProposition(
			const FString& NewLetter,
			const FString& NewSentence
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

		template<typename UserClass>
		void SetToggleMinimizedRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(APropositionBlock*)
		)
		{
			this->OnToggleMinimizedRequested.
				BindUObject(Listener, Callback);
		}

		virtual void LongPressed() override
		{
			Super::LongPressed();

			this->RequestToggleMinimized();
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

		void RequestToggleMinimized()
		{
			bool bHasToggleMinimizedListener =
				this->OnToggleMinimizedRequested.IsBound();

			check(bHasToggleMinimizedListener);

			this->OnToggleMinimizedRequested.Execute(this);
		}

		FOnPropositionToggleMinimizedRequested
			OnToggleMinimizedRequested;

		FString Sentence;

		FString Letter;

		bool bMinimized = false;
};