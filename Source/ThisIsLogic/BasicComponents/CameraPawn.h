#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

#include "CameraPawn.generated.h"

UCLASS()
class THISISLOGIC_API ACameraPawn : public APawn
{
	GENERATED_BODY()

	public:

		ACameraPawn()
		{
			PrimaryActorTick.bCanEverTick = false;

			FName CameraName = TEXT("Camera");

			Camera =
				CreateDefaultSubobject<
					UCameraComponent
				>(CameraName);

			RootComponent = Camera;
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			FVector OverheadLocation =
				FVector(0.f, 0.f, 100.f);

			FRotator LookDownRotation =
				FRotator(-90.f, 0.f, 0.f);

			SetActorLocationAndRotation(
				OverheadLocation, LookDownRotation
			);

			ECameraProjectionMode::Type ProjectionMode =
				ECameraProjectionMode::Orthographic;

			Camera->SetProjectionMode(ProjectionMode);
			Camera->SetOrthoWidth(1000.f);
		}

	private:

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UCameraComponent> Camera;

};
