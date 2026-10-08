#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"

#include "PointerTarget.h"

#include "BackgroundTarget.generated.h"

DECLARE_DELEGATE(FOnBackgroundTapped);

UCLASS()
class THISISLOGIC_API ABackgroundTarget :
	public AActor, public IPointerTarget
{
	GENERATED_BODY()

	public:

		ABackgroundTarget()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName AreaName = TEXT("Area");

			this->Area =
				CreateDefaultSubobject<
					UBoxComponent
				>(AreaName);

			this->RootComponent = this->Area;

			FVector AreaExtent =
				FVector(10000.f, 10000.f, 1.f);

			this->Area->InitBoxExtent(AreaExtent);

			this->Area->
				SetCollisionEnabled(
					ECollisionEnabled::QueryOnly
				);

			this->Area->
				SetCollisionResponseToAllChannels(
					ECR_Ignore
				);

			this->Area->
				SetCollisionResponseToChannel(
					ECC_Visibility, ECR_Block
				);
		}

		template<typename UserClass>
		void SetTappedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnTapped.
				BindUObject(Listener, Callback);
		}

		virtual void Tapped() override
		{
			bool bHasTappedListener =
				this->OnTapped.IsBound();

			check(bHasTappedListener);

			this->OnTapped.Execute();
		}

		virtual void DragStarted(
			const FVector& PressPoint
		)
			override
		{
		}

		virtual void Dragged(const FVector& Point)
			override
		{
		}

		virtual void DragEnded() override
		{
		}

	private:

		FOnBackgroundTapped OnTapped;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UBoxComponent> Area;
};