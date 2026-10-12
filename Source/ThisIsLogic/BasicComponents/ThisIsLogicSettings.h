#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"
#include "Materials/MaterialInterface.h"

#include "../UI/CommandPanel.h"

#include "ThisIsLogicSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class THISISLOGIC_API UThisIsLogicSettings :
	public UDeveloperSettings
{
	GENERATED_BODY()

	public:

		UMaterialInterface*
			GetBlockMaterial() const
		{
			UMaterialInterface* Material =
				this->BlockMaterial.
				LoadSynchronous();

			bool bHasMaterial = Material != nullptr;

			check(bHasMaterial);

			return Material;
		}

		TSubclassOf<UCommandPanel>
			GetCommandPanelClass() const
		{
			TSubclassOf<UCommandPanel> PanelClass =
				this->CommandPanelClass.
				LoadSynchronous();

			bool bHasPanelClass = PanelClass != nullptr;

			check(bHasPanelClass);

			return PanelClass;
		}

	private:

		UPROPERTY(
			Config, EditAnywhere, Category = "Block"
		)
		TSoftObjectPtr<UMaterialInterface>
			BlockMaterial;

		UPROPERTY(
			Config, EditAnywhere, Category = "UI"
		)
		TSoftClassPtr<UCommandPanel>
			CommandPanelClass;
};