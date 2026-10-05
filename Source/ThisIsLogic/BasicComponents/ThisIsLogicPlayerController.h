#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Logging/StructuredLog.h"

#include "../Gameplay/Block/Block.h"
#include "../Gameplay/Formula/Formula.h"
   
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
			ULocalPlayer* LocalPlayer = this->GetLocalPlayer();

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
				this->
					GetHitResultUnderCursor(
						ECC_Visibility, false, Hit
					);

			if (bHasHit)
			{
				AActor* HitActor = Hit.GetActor();

				ABlock* HitBlock = Cast<ABlock>(HitActor);

				bool bHitBlock = HitBlock != nullptr;

				if (bHitBlock)
				{
					FVector GrabPoint = Hit.ImpactPoint;

					this->GrabBlock(HitBlock, GrabPoint);
				}
			}
		}

		void GrabBlock(
			ABlock* HitBlock, const FVector& GrabPoint
		)
		{
			this->DraggedBlock = HitBlock;

			FVector BlockLocation =
				HitBlock->GetActorLocation();

			this->GrabOffset = BlockLocation - GrabPoint;

			FString BlockName = HitBlock->GetName();

			UE_LOGFMT(
				LogTemp, Warning, "Picked: {0}", BlockName
			);
		}

		void OnClickCompleted()
		{
			this->DraggedBlock = nullptr;

			UE_LOGFMT(LogTemp, Warning, "Released");
		}

		void OnClickTriggered()
		{
			bool bIsDragging =
				this->DraggedBlock != nullptr;

			if (bIsDragging)
			{
				this->DragBlock();
			}
		}

		void DragBlock()
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
				AActor* BlockOwner =
					this->DraggedBlock->GetOwner();

				AFormula* Formula = Cast<AFormula>(BlockOwner);

				bool bHasFormula = Formula != nullptr;

				if (bHasFormula)
				{
					this->MoveBlock(
						MouseOrigin, MouseDirection,
						Formula
					);
				}
			}
		}

		void MoveBlock(
			FVector MouseOrigin, FVector MouseDirection,
			AFormula* Formula
		)
		{
			FVector BlockLocation =
				this->DraggedBlock->GetActorLocation();

			FPlane BlockPlane =
				FPlane(BlockLocation, FVector::UpVector);

			FVector MousePoint =
				FMath::RayPlaneIntersection(
					MouseOrigin,
					MouseDirection,
					BlockPlane
				);

			FVector DesiredLocation =
				MousePoint + this->GrabOffset;

			Formula->MoveBlock(
				this->DraggedBlock,
				DesiredLocation.Y
			);
		}

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputMappingContext> MappingContext;

		UPROPERTY(EditDefaultsOnly, Category = "Input")
		TObjectPtr<UInputAction> ClickAction;

		UPROPERTY()
		TObjectPtr<ABlock> DraggedBlock;

		FVector GrabOffset = FVector::ZeroVector;
};
