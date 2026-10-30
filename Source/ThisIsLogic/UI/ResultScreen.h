#pragma once

#include "CoreMinimal.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"

#include "AuxUI.h"

#include "ResultScreen.generated.h"

DECLARE_DELEGATE(FOnOkRequested);

UCLASS(Abstract)
class THISISLOGIC_API UResultScreen :
	public UUserWidget
{
	GENERATED_BODY()

	public:

		template<typename UserClass>
		void SetOkRequestedListener(
			UserClass* Listener, void (UserClass::* Callback)()
		)
		{
			this->OnOkRequested.BindUObject(Listener, Callback);
		}

		void Show(bool bIsSolved)
		{
			FString Message;

			FColor SRGBColor;

			if (bIsSolved)
			{
				Message = "Congratulations!\nYou solved it!";

				SRGBColor = FColor(0xFF22D3EE);
			}
			else
			{
				Message = "Not simple enough!\nTry again?";

				SRGBColor = FColor(0xFFFF0000);
			}

			FText Text = FText::FromString(Message);

			this->MessageText->SetText(Text);

			FLinearColor OutlineColor = FLinearColor(SRGBColor);

			this->SetOutlineColor(OutlineColor);

			this->SetVisibility(
				ESlateVisibility::Visible
			);
		}

	protected:

		virtual void NativeOnInitialized() override
		{
			Super::NativeOnInitialized();

			FAuxUI::
				AssignButtonClick(
					this,
					this->OkButton,
					"OnOkClicked"
				);

			this->SetVisibility(
				ESlateVisibility::Collapsed
			);
		}

		virtual FReply NativeOnMouseButtonDown(
			const FGeometry& InGeometry,
			const FPointerEvent& InMouseEvent
		)
			override
		{
			return FReply::Handled();
		}

		virtual FReply NativeOnMouseButtonDoubleClick(
			const FGeometry& InGeometry,
			const FPointerEvent& InMouseEvent
		)
			override
		{
			return FReply::Handled();
		}

	private:

		UFUNCTION()
		void OnOkClicked()
		{
			this->SetVisibility(
				ESlateVisibility::Collapsed
			);

			bool bHasOkListener = this->OnOkRequested.IsBound();

			check(bHasOkListener);

			this->OnOkRequested.Execute();
		}

		void SetOutlineColor(const FLinearColor& Color)
		{
			FSlateColor OutlineColor = FSlateColor(Color);

			FSlateBrush Brush = this->ResultBorder->Background;

			Brush.OutlineSettings.Color = OutlineColor;

			this->ResultBorder->SetBrush(Brush);

			FButtonStyle Style = this->OkButton->GetStyle();

			Style.Normal.OutlineSettings.Color = OutlineColor;

			Style.Hovered.OutlineSettings.Color = OutlineColor;

			Style.Pressed.OutlineSettings.Color = OutlineColor;

			this->OkButton->SetStyle(Style);
		}

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UBorder> ResultBorder;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> MessageText;

		UPROPERTY(meta = (BindWidget))
		TObjectPtr<UButton> OkButton;

		FOnOkRequested OnOkRequested;
};