#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

#include "Block.h"
#include "BlockBoard.h"

#include "AbstractBlocksManager.generated.h"

UCLASS(Abstract)
class THISISLOGIC_API AAbstractBlocksManager : public AActor
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		AAbstractBlocksManager()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName RootName = TEXT("Root");

			USceneComponent* Root =
				CreateDefaultSubobject<USceneComponent>(RootName);

			this->RootComponent = Root;

			FName BoardName = TEXT("Board");

			this->Board =
				CreateDefaultSubobject<UBlockBoard>(BoardName);
		}

		void SetMaxRowWidth(double MaxRowWidth)
		{
			double RowHeightRatio = this->GetRowHeightRatio();

			this->Board->
				SetData(
					RowHeightRatio,
					AAbstractBlocksManager::BlockHeight,
					MaxRowWidth
				);
		}

	#pragma endregion

	#pragma region ProtectedMethods
	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			FVector Location = this->GetActorLocation();

			this->Board->SetOrigin(Location);

			this->ConnectBoard();
		}

		virtual double GetRowHeightRatio() const
		{
			unimplemented();

			return 0.0;
		}
	
		virtual void OnBlockToggleRequested(ABlock* Block)
		{
		}
	
		virtual void OnBlockSelectRequested(ABlock* Block)
		{
			unimplemented();
		}

		virtual void OnBlockMoveRequested(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
		}

		virtual void OnBlockDropRequested(ABlock* Block)
		{
		}

		static constexpr float BlockHeight = 2.f;

	#pragma endregion
	
	#pragma region ProtectedAttributes

		UPROPERTY()
		TObjectPtr<UBlockBoard> Board;

	#pragma endregion

	private:
	
		void ConnectBoard()
		{
			this->Board->
				SetSelectRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockSelectRequested
				);

			this->Board->
				SetMoveRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockMoveRequested
				);

			this->Board->
				SetDropRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockDropRequested
				);
			
			this->Board->
				SetToggleRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockToggleRequested
				);
		}
};