#pragma once

#include "CoreMinimal.h"
#include "UnrealClient.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Engine/EngineTypes.h"
#include "Logging/StructuredLog.h"
#include "Camera/CameraComponent.h"
#include "Engine/GameViewportClient.h"

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

			EAspectRatioAxisConstraint AxisConstraint =
				AspectRatio_MaintainXFOV;

			this->Camera->
				bOverrideAspectRatioAxisConstraint = true;

			this->Camera->
				SetAspectRatioAxisConstraint(AxisConstraint);
			
			this->SetPostProcess();
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

			this->FitToViewport();

			FViewport::ViewportResizedEvent.
				AddUObject(this, &ACameraPawn::OnViewportResized);
		}

		virtual void EndPlay(
			const EEndPlayReason::Type EndPlayReason
		)
			override
		{
			FViewport::
				ViewportResizedEvent.RemoveAll(this);

			Super::EndPlay(EndPlayReason);
		}

	private:

		void OnViewportResized(
			FViewport* Viewport, uint32 Unused
		)
		{
			this->FitToViewport();
		}

		void FitToViewport()
		{
			UWorld* World = this->GetWorld();

			UGameViewportClient* ViewportClient =
				World->GetGameViewport();

			FVector2D ViewportSize = FVector2D::ZeroVector;

			ViewportClient->GetViewportSize(ViewportSize);

			double SizeX = ViewportSize.X;

			double SizeY = ViewportSize.Y;

			bool bHasHeight = SizeY > 0.0;

			if (bHasHeight)
			{
				double AspectRatio = SizeX / SizeY;

				this->FitToAspectRatio(AspectRatio);
			}
		}

		void FitToAspectRatio(double AspectRatio)
		{
			double WidthForMinHeight =
				ACameraPawn::MinViewHeight * AspectRatio;

			double OrthoWidth =
				FMath::Max(
					ACameraPawn::MinViewWidth, 
					WidthForMinHeight
				);

			float CameraOrthoWidth =
				static_cast<float>(OrthoWidth);

			this->Camera->SetOrthoWidth(CameraOrthoWidth);

			UE_LOGFMT(
				LogTemp, Warning,
				"Ortho width: {0}", CameraOrthoWidth
			);
		}

		void SetPostProcess()
		{
			FPostProcessSettings& Settings =
				this->Camera->PostProcessSettings;

			Settings.bOverride_ToneCurveAmount = true;

			Settings.ToneCurveAmount = 0.f;

			Settings.bOverride_ExpandGamut = true;

			Settings.ExpandGamut = 0.f;

			Settings.bOverride_VignetteIntensity = true;

			Settings.VignetteIntensity = 0.f;
		}
	
		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UCameraComponent> Camera;

		static constexpr double MinViewWidth = 50.0;

		static constexpr double MinViewHeight = 28.0;
};