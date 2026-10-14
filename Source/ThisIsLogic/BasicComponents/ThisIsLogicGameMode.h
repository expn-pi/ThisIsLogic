#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "CameraPawn.h"

#include "ThisIsLogicGameMode.generated.h"

UCLASS()
class THISISLOGIC_API AThisIsLogicGameMode :
	public AGameModeBase
{
	GENERATED_BODY()

	public:

		AThisIsLogicGameMode()
		{
			DefaultPawnClass = ACameraPawn::StaticClass();
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();
		}
};