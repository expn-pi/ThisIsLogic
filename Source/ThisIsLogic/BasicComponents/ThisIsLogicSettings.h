#pragma once

#include "CoreMinimal.h"
#include "Engine/Font.h"
#include "Engine/DataTable.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/DeveloperSettings.h"
#include "Materials/MaterialInterface.h"

#include "../UI/CommandPanel.h"

#include "ThisIsLogicSettings.generated.h"

UCLASS(Config = Game, DefaultConfig)
class THISISLOGIC_API UThisIsLogicSettings :
	public UDeveloperSettings
{
	GENERATED_BODY()

	public:

		UFont* GetBlockFont() const
		{
			UFont* Font = this->BlockFont.LoadSynchronous();

			bool bHasFont = Font != nullptr;

			check(bHasFont);

			return Font;
		}
	
		UMaterialInterface* GetBlockMaterial() const
		{
			UMaterialInterface* Material =
				this->BlockMaterial.LoadSynchronous();

			bool bHasMaterial = Material != nullptr;

			check(bHasMaterial);

			return Material;
		}
	
		UMaterialInterface* GetTextMaterial() const
		{
			UMaterialInterface* Material =
				this->TextMaterial.LoadSynchronous();

			bool bHasMaterial = Material != nullptr;

			check(bHasMaterial);

			return Material;
		}

		TSubclassOf<UCommandPanel> GetCommandPanelClass()
			const
		{
			TSubclassOf<UCommandPanel> PanelClass =
				this->CommandPanelClass.LoadSynchronous();

			bool bHasPanelClass = PanelClass != nullptr;

			check(bHasPanelClass);

			return PanelClass;
		}

		UDataTable* GetLevelTable() const
		{
			UDataTable* Table =
				this->LevelTable.LoadSynchronous();

			bool bHasTable = Table != nullptr;

			check(bHasTable);

			return Table;
		}

	private:

		UPROPERTY(
			Config, EditAnywhere, Category = "Block"
		)
		TSoftObjectPtr<UFont>
			BlockFont;
	
		UPROPERTY(
			Config, EditAnywhere, Category = "Block"
		)
		TSoftObjectPtr<UMaterialInterface>
			BlockMaterial;
	
		UPROPERTY(
			Config, EditAnywhere, Category = "Block"
		)
		TSoftObjectPtr<UMaterialInterface>
			TextMaterial;

		UPROPERTY(
			Config, EditAnywhere, Category = "UI"
		)
		TSoftClassPtr<UCommandPanel>
			CommandPanelClass;

		UPROPERTY(
			Config, EditAnywhere, Category = "Level"
		)
		TSoftObjectPtr<UDataTable>
			LevelTable;
};