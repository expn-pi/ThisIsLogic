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
			
			this->Board->SetupAttachment(Root);
		}

		void SetMaxRowWidth(double MaxRowWidth)
		{
			double RowHeightRatio = this->GetRowHeightRatio();

			int32 BackgroundRowCount =
				this->GetBackgroundRowCount();

			this->Board->
				SetData(
					RowHeightRatio,
					AAbstractBlocksManager::BlockHeight,
					MaxRowWidth,
					BackgroundRowCount
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
	
		virtual int32 GetBackgroundRowCount() const
		{
			unimplemented();

			return 0;
		}
	
		virtual void OnBlockTapped(ABlock* Block)
		{
			unimplemented();
		}

		virtual void OnBlockLongPressed(ABlock* Block)
		{
			unimplemented();
		}

		virtual void OnBlockDragStarted(ABlock* Block)
		{
		}

		virtual void OnBlockDragged(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
		}

		virtual void OnBlockDragEnded(ABlock* Block)
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
				SetTappedListener(
					this,
					&AAbstractBlocksManager::OnBlockTapped
				);

			this->Board->
				SetLongPressedListener(
					this,
					&AAbstractBlocksManager::OnBlockLongPressed
				);

			this->Board->
				SetDragStartedListener(
					this,
					&AAbstractBlocksManager::OnBlockDragStarted
				);

			this->Board->
				SetDraggedListener(
					this,
					&AAbstractBlocksManager::OnBlockDragged
				);

			this->Board->
				SetDragEndedListener(
					this,
					&AAbstractBlocksManager::OnBlockDragEnded
				);
		}
};