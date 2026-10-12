#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"

#include "CommandPanel.generated.h"

DECLARE_DELEGATE_OneParam(FOnAllMinimizedRequested, bool);

DECLARE_DELEGATE(FOnToggleMinimizedRequested);

UCLASS(Abstract)
class THISISLOGIC_API UCommandPanel :
	public UUserWidget
{
	GENERATED_BODY()

	public:

		template<typename UserClass>
		void SetAllMinimizedRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(bool)
		)
		{
			this->OnAllMinimizedRequested.
				BindUObject(Listener, Callback);
		}
	
		template<typename UserClass>
		void SetToggleMinimizedRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnToggleMinimizedRequested.
				BindUObject(Listener, Callback);
		}
	
		void SetToggleMinimizedVisible(bool bVisible)
		{
			ESlateVisibility NewVisibility =
				ESlateVisibility::Collapsed;

			if (bVisible)
			{
				NewVisibility = ESlateVisibility::Visible;
			}

			this->ToggleMinimizedButton->
				SetVisibility(NewVisibility);
		}

	protected:

		virtual void NativeOnInitialized() override
		{
			Super::NativeOnInitialized();

			FOnButtonClickedEvent& MinimizeAllClicked =
				this->MinimizeAllButton->OnClicked;

			MinimizeAllClicked.AddDynamic(
				this,
				&UCommandPanel::OnMinimizeAllClicked
			);

			FOnButtonClickedEvent& MaximizeAllClicked =
				this->MaximizeAllButton->OnClicked;

			MaximizeAllClicked.AddDynamic(
				this,
				&UCommandPanel::OnMaximizeAllClicked
			);
			
			FOnButtonClickedEvent& ToggleMinimizedClicked =
				this->ToggleMinimizedButton->OnClicked;

			ToggleMinimizedClicked.AddDynamic(
				this,
				&UCommandPanel::OnToggleMinimizedClicked
			);
		}

	private:

		UFUNCTION()
		void OnMinimizeAllClicked()
		{
			this->RequestAllMinimized(true);
		}

		UFUNCTION()
		void OnMaximizeAllClicked()
		{
			this->RequestAllMinimized(false);
		}
	
		UFUNCTION()
		void OnToggleMinimizedClicked()
		{
			bool bHasToggleMinimizedListener =
				this->OnToggleMinimizedRequested.IsBound();

			check(bHasToggleMinimizedListener);

			this->OnToggleMinimizedRequested.Execute();
		}

		void RequestAllMinimized(bool bMinimized)
		{
			bool bHasAllMinimizedListener =
				this->OnAllMinimizedRequested.IsBound();

			check(bHasAllMinimizedListener);

			this->OnAllMinimizedRequested.Execute(bMinimized);
		}

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> MinimizeAllButton;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> MaximizeAllButton;
	
		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> ToggleMinimizedButton;
		
		FOnAllMinimizedRequested OnAllMinimizedRequested;
	
		FOnToggleMinimizedRequested OnToggleMinimizedRequested;
};