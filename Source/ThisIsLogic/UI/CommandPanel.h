#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"

#include "AuxUI.h"

#include "CommandPanel.generated.h"

DECLARE_DELEGATE_OneParam(FOnAllMinimizedRequested, bool);

DECLARE_DELEGATE(FOnToggleMinimizedRequested);

DECLARE_DELEGATE(FOnRemoveRequested);

DECLARE_DELEGATE(FOnRestartRequested);

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
	
		template<typename UserClass>
		void SetRestartRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnRestartRequested.
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

			FAuxUI::
				AssignButtonClick(
					this,
					this->MinimizeAllButton,
					"OnMinimizeAllClicked"
				);
			
			FAuxUI::
				AssignButtonClick(
					this,
					this->MaximizeAllButton,
					"OnMaximizeAllClicked"
				);
			
			FAuxUI::
				AssignButtonClick(
					this,
					this->ToggleMinimizedButton,
					"OnToggleMinimizedClicked"
				);
			
			FAuxUI::
				AssignButtonClick(
					this,
					this->RemoveButton,
					"OnRemoveClicked"
				);
			
			FAuxUI::
				AssignButtonClick(
					this,
					this->RestartButton,
					"OnRestartClicked"
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
		
		UFUNCTION()
		void OnRestartClicked()
		{
			bool bHasRestartListener =
				this->OnRestartRequested.IsBound();

			check(bHasRestartListener);

			this->OnRestartRequested.Execute();
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
		
		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> RestartButton;
		
		FOnAllMinimizedRequested OnAllMinimizedRequested;
	
		FOnToggleMinimizedRequested
			OnToggleMinimizedRequested;

		FOnRemoveRequested OnRemoveRequested;
		
		FOnRestartRequested OnRestartRequested;
	
	#pragma endregion
};