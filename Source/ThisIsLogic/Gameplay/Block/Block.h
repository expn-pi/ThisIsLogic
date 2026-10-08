#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"

#include "../../Input/PointerTarget.h"
#include "../../Visuals/RoundedBackgroundComponent.h"
#include "../../BasicComponents/ThisIsLogicSettings.h"

#include "Block.generated.h"

class ABlock;

DECLARE_DELEGATE_OneParam(FOnBlockWidthChanged, ABlock*);

DECLARE_DELEGATE_TwoParams(
	FOnBlockMoveRequested, ABlock*, const FVector&
);

DECLARE_DELEGATE_OneParam(FOnBlockDropRequested, ABlock*);

DECLARE_DELEGATE_OneParam(FOnBlockSelectRequested, ABlock*);

UCLASS(Abstract)
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

			this->Background->SetRoundness(1.f);

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

			FName OutlineName = TEXT("Outline");

			this->Outline =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(OutlineName);

			this->Outline->
				SetupAttachment(this->Background);

			FVector OutlineLocation =
				FVector(0.f, 0.f, -1.f);

			this->Outline->
				SetRelativeLocation(OutlineLocation);

			this->Outline->SetRoundness(1.f);

			FLinearColor OutlineColor =
				FLinearColor::Yellow;

			this->Outline->SetColor(OutlineColor);

			this->Outline->
				SetCollisionEnabled(
					ECollisionEnabled::NoCollision
				);

			this->Outline->SetVisibility(false);
		}

		virtual void OnConstruction(
			const FTransform& Transform
		)
			override
		{
			Super::OnConstruction(Transform);

			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UMaterialInterface* BlockMaterial =
				Settings->GetBlockMaterial();

			this->Background->
				SetBaseMaterial(BlockMaterial);

			this->Background->
				Build(this->Width, this->Height);

			this->Outline->
				SetBaseMaterial(BlockMaterial);

			this->Outline->
				Build(this->Width, this->Height);

			this->ApplyText();
		}

		float GetWidth() const
		{
			return this->Width;
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

		template<typename UserClass>
		void SetSelectRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnSelectRequested.
				BindUObject(Listener, Callback);
		}

		void SetSelected(bool bNewSelected)
		{
			this->Outline->SetVisibility(bNewSelected);
		}

		virtual void Tapped() override
		{
			this->RequestSelection();
		}

		virtual void DragStarted(
			const FVector& PressPoint
		)
			override
		{
			this->RequestSelection();

			FVector Location = this->GetActorLocation();

			this->GrabOffset = Location - PressPoint;
		}

		virtual void Dragged(const FVector& Point)
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

		virtual void DragEnded() override
		{
			bool bHasDropListener =
				this->OnDropRequested.IsBound();

			check(bHasDropListener);

			this->OnDropRequested.Execute(this);
		}

	protected:

		void SetColor(const FLinearColor& NewColor)
		{
			this->Background->SetColor(NewColor);
		}

		virtual FString GetText() const
		{
			unimplemented();

			return FString();
		}

		void FitToText()
		{
			this->ApplyText();

			bool bHasWidthListener =
				this->OnWidthChanged.IsBound();

			check(bHasWidthListener);

			this->OnWidthChanged.Execute(this);
		}

	private:

		void ApplyText()
		{
			FString BlockText = this->GetText();

			FText LabelText =
				FText::FromString(BlockText);

			this->Label->SetText(LabelText);

			this->Width = this->GetWidthForText();

			this->Background->
				Resize(this->Width, this->Height);

			this->ResizeOutline();
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

		void ResizeOutline()
		{
			float Border = this->OutlineThickness * 2.f;

			float OutlineWidth = this->Width + Border;

			float OutlineHeight = this->Height + Border;

			this->Outline->
				Resize(OutlineWidth, OutlineHeight);
		}

		void RequestSelection()
		{
			bool bHasSelectListener =
				this->OnSelectRequested.IsBound();

			check(bHasSelectListener);

			this->OnSelectRequested.Execute(this);
		}

		FOnBlockWidthChanged OnWidthChanged;

		FOnBlockMoveRequested OnMoveRequested;

		FOnBlockDropRequested OnDropRequested;

		FOnBlockSelectRequested OnSelectRequested;

		FVector GrabOffset = FVector::ZeroVector;

		// Text

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> Label;

		UPROPERTY(EditAnywhere, Category = "Block")
		float TextMargin = 20.f;

		// Background

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Background;

		// Selection

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Outline;

		UPROPERTY(EditAnywhere, Category = "Block")
		float OutlineThickness = 8.f;

		// Block

		UPROPERTY(VisibleAnywhere, Category = "Block")
		float Width = 100.f;

		UPROPERTY(EditAnywhere, Category = "Block")
		float Height = 100.f;
};