#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Logging/StructuredLog.h"
#include "UObject/ScriptInterface.h"

#include "../Input/PointerTarget.h"

#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"
#include "ThisIsLogicPlayerController.generated.h"


UCLASS()
class THISISLOGIC_API AThisIsLogicPlayerController :
	public APlayerController
{
	GENERATED_BODY()

	public:

		AThisIsLogicPlayerController()
		{
			this->bShowMouseCursor = true;
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			bool bHasMappingContext =
				this->MappingContext != nullptr;

			if (bHasMappingContext)
			{
				this->RegisterMappingContext();
			}
		}

		virtual void SetupInputComponent() override
		{
			Super::SetupInputComponent();

			bool bHasClickAction =
				this->ClickAction != nullptr;

			if (bHasClickAction)
			{
				this->BindInputActions();
			}
		}

	private:

		void RegisterMappingContext()
		{
			ULocalPlayer* LocalPlayer =
				this->GetLocalPlayer();

			UEnhancedInputLocalPlayerSubsystem*
				InputSubsystem =
					LocalPlayer->
						GetSubsystem<
							UEnhancedInputLocalPlayerSubsystem
						>();

			InputSubsystem->
				AddMappingContext(this->MappingContext, 0);
		}

		void BindInputActions()
		{
			UEnhancedInputComponent* EnhancedInput =
				Cast<UEnhancedInputComponent>(
					this->InputComponent
				);

			EnhancedInput->
				BindAction(
					this->ClickAction,
					ETriggerEvent::Started,
					this,
					&AThisIsLogicPlayerController::
					OnClickStarted
				);

			EnhancedInput->
				BindAction(
					this->ClickAction,
					ETriggerEvent::Completed,
					this,
					&AThisIsLogicPlayerController::
					OnClickCompleted
				);

			EnhancedInput->
				BindAction(
					this->ClickAction,
					ETriggerEvent::Triggered,
					this,
					&AThisIsLogicPlayerController::
					OnClickTriggered
				);
		}

		void OnClickStarted()
		{
			FHitResult Hit;

			bool bHasHit =
				this->GetHitResultUnderCursor(
					ECC_Visibility, false, Hit
				);

			if (bHasHit)
			{
				AActor* HitActor = Hit.GetActor();

				IPointerTarget* PointerTarget =
					Cast<IPointerTarget>(HitActor);

				bool bIsPointerTarget =
					PointerTarget != nullptr;

				if (bIsPointerTarget)
				{
					FVector PressPoint = Hit.ImpactPoint;

					this->PointerPressed(
						HitActor, PressPoint
					);
				}
			}
		}

		void PointerPressed(
			AActor* Target, const FVector& PressPoint
		)
		{
			this->PressedTarget = Target;

			this->PressPlane =
				FPlane(PressPoint, FVector::UpVector);

			this->PressedTarget->PointerPressed(PressPoint);

			FString TargetName = Target->GetName();

			UE_LOGFMT(
				LogTemp, Warning, "Pressed: {0}", TargetName
			);
		}

		void OnClickTriggered()
		{
			bool bHasPressedTarget =
				this->HasPressedTarget();

			if (bHasPressedTarget)
			{
				this->PointerHeld();
			}
		}

		void PointerHeld()
		{
			FVector MouseOrigin;
			FVector MouseDirection;

			bool bHasMouse =
				this->DeprojectMousePositionToWorld(
					MouseOrigin,
					MouseDirection
				);

			if (bHasMouse)
			{
				FVector PointerPoint =
					FMath::RayPlaneIntersection(
						MouseOrigin,
						MouseDirection,
						this->PressPlane
					);

				this->PressedTarget->
					PointerHeld(PointerPoint);
			}
		}

		void OnClickCompleted()
		{
			bool bHasPressedTarget =
				this->HasPressedTarget();

			if (bHasPressedTarget)
			{
				this->PointerReleased();
			}
		}

		void PointerReleased()
		{
			this->PressedTarget->PointerReleased();

			this->PressedTarget = nullptr;

			UE_LOGFMT(LogTemp, Warning, "Released");
		}

		bool HasPressedTarget() const
		{
			UObject* PressedObject =
				this->PressedTarget.GetObject();

			return PressedObject != nullptr;
		}

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> MappingContext;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputAction> ClickAction;

		UPROPERTY()
		TScriptInterface<IPointerTarget> PressedTarget;

		FPlane PressPlane = FPlane(ForceInit);
};