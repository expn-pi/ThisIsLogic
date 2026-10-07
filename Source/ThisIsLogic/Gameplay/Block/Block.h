#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"

#include "../../Input/PointerTarget.h"
#include "../../Visuals/RoundedBackgroundComponent.h"

#include "Block.generated.h"

class ABlock;

DECLARE_DELEGATE_OneParam(FOnBlockWidthChanged, ABlock*);

DECLARE_DELEGATE_TwoParams(
	FOnBlockMoveRequested, ABlock*, const FVector&
);

DECLARE_DELEGATE_OneParam(FOnBlockDropRequested, ABlock*);

UCLASS()
class THISISLOGIC_API ABlock :
	public AActor, public IPointerTarget
{
	GENERATED_BODY()

	public:

		ABlock()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName BackgroundName = TEXT("Background");

			this->Background =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(BackgroundName);

			this->RootComponent = this->Background;

			FName LabelName = TEXT("Label");

			this->Label =
				CreateDefaultSubobject<
					UTextRenderComponent
				>(LabelName);

			this->Label->SetupAttachment(this->Background);

			FVector LabelLocation = FVector(0.f, 0.f, 1.f);

			FRotator LabelRotation =
				FRotator(90.f, 180.f, 0.f);

			this->Label->
				SetRelativeLocationAndRotation(
					LabelLocation, LabelRotation
				);

			this->Label->
				SetHorizontalAlignment(EHTA_Center);

			this->Label->
				SetVerticalAlignment(EVRTA_TextCenter);

			this->Label->SetWorldSize(50.f);
		}

		virtual void OnConstruction(
			const FTransform& Transform
		)
			override
		{
			Super::OnConstruction(Transform);

			this->ApplyText();

			this->Background->
				Build(this->Width, this->Height);
		}

		float GetWidth() const
		{
			return this->Width;
		}

		void SetText(const FString& NewText)
		{
			this->Text = NewText;
		}

		template<typename UserClass>
		void SetWidthChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnWidthChanged.
				BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetMoveRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(
				ABlock*, const FVector&
			)
		)
		{
			this->OnMoveRequested.
				BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetDropRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnDropRequested.
				BindUObject(Listener, Callback);
		}

		virtual void PointerPressed(
			const FVector& Point
		)
			override
		{
			FVector Location = this->GetActorLocation();

			this->GrabOffset = Location - Point;
		}

		virtual void PointerHeld(const FVector& Point)
			override
		{
			bool bHasMoveListener =
				this->OnMoveRequested.IsBound();

			check(bHasMoveListener);

			FVector DesiredLocation =
				Point + this->GrabOffset;

			this->OnMoveRequested.
				Execute(this, DesiredLocation);
		}

		virtual void PointerReleased() override
		{
			bool bHasDropListener =
				this->OnDropRequested.IsBound();

			check(bHasDropListener);

			this->OnDropRequested.Execute(this);
		}

	private:

		void ApplyText()
		{
			FText LabelText =
				FText::FromString(this->Text);

			this->Label->SetText(LabelText);

			this->Width = this->GetWidthForText();
		}

		float GetWidthForText() const
		{
			FVector TextSize =
				this->Label->GetTextLocalSize();

			float TextWidth =
				static_cast<float>(TextSize.Y);

			float Margins = this->TextMargin * 2.f;

			float WidthWithMargins = TextWidth + Margins;

			return
				FMath::Max(WidthWithMargins, this->Height);
		}

		void SetWidth(float NewWidth)
		{
			this->Width = NewWidth;

			this->Background->
				Resize(this->Width, this->Height);

			this->OnWidthChanged.Execute(this);
		}

		FOnBlockWidthChanged OnWidthChanged;

		FOnBlockMoveRequested OnMoveRequested;

		FOnBlockDropRequested OnDropRequested;

		FVector GrabOffset = FVector::ZeroVector;

		// Text

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> Label;

		UPROPERTY(EditAnywhere, Category = "Block")
		FString Text = TEXT("A");

		UPROPERTY(EditAnywhere, Category = "Block")
		float TextMargin = 20.f;

		// Background

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Background;

		// Block

		UPROPERTY(VisibleAnywhere, Category = "Block")
		float Width = 100.f;

		UPROPERTY(EditAnywhere, Category = "Block")
		float Height = 100.f;
};