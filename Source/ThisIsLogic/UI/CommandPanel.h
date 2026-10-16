#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"

#include "CommandPanel.generated.h"

DECLARE_DELEGATE_OneParam(FOnAllMinimizedRequested, bool);

DECLARE_DELEGATE(FOnToggleMinimizedRequested);

DECLARE_DELEGATE(FOnRemoveRequested);

UCLASS(Abstract)
class THISISLOGIC_API UCommandPanel :
	public UUserWidget
{
	GENERATED_BODY()
	
	#pragma region PublicMethods
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

		template<typename UserClass>
		void SetRemoveRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnRemoveRequested.
				BindUObject(Listener, Callback);
		}

		void SetSelectionButtonsVisible(bool bVisible)
		{
			ESlateVisibility NewVisibility =
				ESlateVisibility::Collapsed;

			if (bVisible)
			{
				NewVisibility =
					ESlateVisibility::SelfHitTestInvisible;
			}

			this->SelectionButtonsBox->
				SetVisibility(NewVisibility);
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

	#pragma endregion
	
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

			FOnButtonClickedEvent& RemoveClicked =
				this->RemoveButton->OnClicked;

			RemoveClicked.AddDynamic(
				this,
				&UCommandPanel::OnRemoveClicked
			);
		}

	#pragma region PrivateMethods
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

		UFUNCTION()
		void OnRemoveClicked()
		{
			bool bHasRemoveListener =
				this->OnRemoveRequested.IsBound();

			check(bHasRemoveListener);

			this->OnRemoveRequested.Execute();
		}

		void RequestAllMinimized(bool bMinimized)
		{
			bool bHasAllMinimizedListener =
				this->OnAllMinimizedRequested.IsBound();

			check(bHasAllMinimizedListener);

			this->OnAllMinimizedRequested.
				Execute(bMinimized);
		}
	
	#pragma endregion
	
	#pragma region Attributes
	
		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> MinimizeAllButton;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> MaximizeAllButton;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UHorizontalBox> SelectionButtonsBox;
	
		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> ToggleMinimizedButton;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> RemoveButton;
		
		FOnAllMinimizedRequested OnAllMinimizedRequested;
	
		FOnToggleMinimizedRequested
			OnToggleMinimizedRequested;

		FOnRemoveRequested OnRemoveRequested;
	
	#pragma endregion
};