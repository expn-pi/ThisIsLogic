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
					FVector HitPoint = Hit.ImpactPoint;

					this->PointerPressed(
						HitActor, HitPoint
					);
				}
			}
		}

		void PointerPressed(
			AActor* Target, const FVector& Point
		)
		{
			this->PressedTarget = Target;

			this->PressPoint = Point;

			this->PressPlane =
				FPlane(Point, FVector::UpVector);

			FVector2D ScreenPosition =
				FVector2D::ZeroVector;

			this->GetMouseScreenPosition(ScreenPosition);

			this->PressScreenPosition = ScreenPosition;

			this->bIsDragging = false;

			FString TargetName = Target->GetName();

			UE_LOGFMT(
				LogTemp, Warning,
				"Pressed: {0}", TargetName
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
			if (this->bIsDragging)
			{
				this->Drag();
			}
			else
			{
				bool bIsPastThreshold =
					this->IsPastDragThreshold();

				if (bIsPastThreshold)
				{
					this->StartDrag();
				}
			}
		}

		bool IsPastDragThreshold() const
		{
			FVector2D ScreenPosition =
				FVector2D::ZeroVector;

			bool bHasMouse =
				this->GetMouseScreenPosition(
					ScreenPosition
				);

			double Distance =
				FVector2D::Distance(
					this->PressScreenPosition,
					ScreenPosition
				);

			double Threshold =
				AThisIsLogicPlayerController::
					DragThreshold;

			bool bIsPastThreshold =
				Distance > Threshold;

			return bHasMouse && bIsPastThreshold;
		}

		void StartDrag()
		{
			this->bIsDragging = true;

			this->PressedTarget->
				DragStarted(this->PressPoint);

			UE_LOGFMT(LogTemp, Warning, "Drag started");

			this->Drag();
		}

		void Drag()
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
					FMath::
						RayPlaneIntersection(
							MouseOrigin,
							MouseDirection,
							this->PressPlane
						);

				this->PressedTarget->
					Dragged(PointerPoint);
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
			if (this->bIsDragging)
			{
				this->PressedTarget->DragEnded();

				UE_LOGFMT(LogTemp, Warning, "Drag ended");
			}
			else
			{
				this->PressedTarget->Tapped();

				UE_LOGFMT(LogTemp, Warning, "Tapped");
			}

			this->PressedTarget = nullptr;
		}

		bool HasPressedTarget() const
		{
			UObject* PressedObject =
				this->PressedTarget.GetObject();

			return PressedObject != nullptr;
		}

		bool GetMouseScreenPosition(
			FVector2D& ScreenPosition
		) const
		{
			float MouseX = 0.f;
			float MouseY = 0.f;

			bool bHasMouse =
				this->GetMousePosition(MouseX, MouseY);

			ScreenPosition = FVector2D(MouseX, MouseY);

			return bHasMouse;
		}

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> MappingContext;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputAction> ClickAction;

		UPROPERTY()
		TScriptInterface<IPointerTarget> PressedTarget;

		FVector PressPoint = FVector::ZeroVector;

		FPlane PressPlane = FPlane(ForceInit);

		FVector2D PressScreenPosition = FVector2D::ZeroVector;

		bool bIsDragging = false;

		static constexpr double DragThreshold = 10.0;
};