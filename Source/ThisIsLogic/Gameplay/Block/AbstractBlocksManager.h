#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

#include "Block.h"
#include "BlockData.h"
#include "BlockBoard.h"
#include "BlockFactory.h"
#include "../../Logic/LogicTypes.h"

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
		}

		void SetMaxRowWidth(double MaxRowWidth)
		{
			double RowHeightRatio = this->GetRowHeightRatio();

			this->Board.
				SetData(
					RowHeightRatio,
					AAbstractBlocksManager::BlockHeight,
					MaxRowWidth
				);
		}

		void SetSentences(const TArray<FString>& NewSentences)
		{
			this->Sentences = NewSentences;
		}

		TArray<FToken> GetTokens() const
		{
			return this->Board.GetTokens();
		}

		TArray<FBlockState> GetStates() const
		{
			return this->Board.GetStates();
		}

	#pragma endregion

	#pragma region ProtectedMethods
	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			FVector Location = this->GetActorLocation();

			this->Board.SetOrigin(Location);
		}

		virtual double GetRowHeightRatio() const
		{
			unimplemented();

			return 0.0;
		}

		void CreateBlocks(const TArray<FToken>& Tokens)
		{
			UWorld* World = this->GetWorld();

			TArray<ABlock*> Blocks =
				this->BlockFactory.
					CreateBlocks(World, Tokens, this->Sentences);

			for (ABlock* Block : Blocks)
			{
				this->RegisterBlock(Block);
			}

			this->Board.AddBlocks(Blocks);
		}

		void CreateBlock(const FToken& Token)
		{
			ABlock* Block = this->CreateRegisteredBlock(Token);

			this->Board.AddBlock(Block);
		}

		void CreateBlockBefore(
			const FToken& Token, ABlock* NextBlock
		)
		{
			ABlock* Block = this->CreateRegisteredBlock(Token);

			this->Board.AddBlockBefore(Block, NextBlock);
		}

		void RemoveBlock(ABlock* Block)
		{
			this->Board.RemoveBlock(Block);
		}

		void RemoveAllBlocks()
		{
			this->Board.RemoveAllBlocks();
		}

		void SetPropositionsMinimized(bool bMinimized)
		{
			this->Board.SetAllMinimized(bMinimized);
		}

		void ApplyStates(const TArray<FBlockState>& States)
		{
			this->Board.ApplyStates(States);
		}

		void DragBlock(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
			this->Board.DragBlock(Block, DesiredLocation);
		}

		void LayoutBlocks()
		{
			this->Board.LayoutBlocks();
		}

		virtual void RegisterBlock(ABlock* Block)
		{
			Block->SetOwner(this);

			Block->
				SetWidthChangedListener(
					this,
					&AAbstractBlocksManager::OnBlockWidthChanged
				);

			Block->
				SetMoveRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockMoveRequested
				);

			Block->
				SetDropRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockDropRequested
				);

			Block->
				SetSelectRequestedListener(
					this,
					&AAbstractBlocksManager::OnBlockSelectRequested
				);
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

	private:

		ABlock* CreateRegisteredBlock(const FToken& Token)
		{
			UWorld* World = this->GetWorld();

			ABlock* Block =
				this->BlockFactory.
					CreateBlock(World, Token, this->Sentences);

			this->RegisterBlock(Block);

			return Block;
		}
		
		void OnBlockWidthChanged(ABlock* Block)
		{
			this->Board.UpdateLayout();
		}

	#pragma region Attributes

		UPROPERTY()
		FBlockBoard Board;

		TArray<FString> Sentences;

		FBlockFactory BlockFactory =
			FBlockFactory(AAbstractBlocksManager::BlockHeight);

	#pragma endregion
};