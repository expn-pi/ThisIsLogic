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
			this->PrimaryActorTick.bCanEverTick = false;

			FName CameraName = TEXT("Camera");

			this->Camera =
				CreateDefaultSubobject<
					UCameraComponent
				>(CameraName);

			this->RootComponent = this->Camera;

			ECameraProjectionMode::Type ProjectionMode =
				ECameraProjectionMode::Orthographic;

			this->Camera->SetProjectionMode(ProjectionMode);

			this->Camera->SetOrthoWidth(50.f);
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			FVector OverheadLocation =
				FVector(0.f, 0.f, 50.f);

			FRotator LookDownRotation =
				FRotator(-90.f, 0.f, 0.f);

			this->SetActorLocationAndRotation(
				OverheadLocation, LookDownRotation
			);
		}

	private:

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UCameraComponent> Camera;
};