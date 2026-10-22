#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Block.h"
#include "BlockData.h"
#include "BlockFactory.h"
#include "BlockBoardRows.h"
#include "PropositionBlock.h"
#include "../../Logic/LogicTypes.h"

#include "BlockBoard.generated.h"

DECLARE_DELEGATE_OneParam(FOnBoardToggleRequested, ABlock*);

UCLASS()
class THISISLOGIC_API UBlockBoard : public UObject
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		void SetOrigin(const FVector& Origin)
		{
			this->Rows.SetOrigin(Origin);
		}

		void SetData(
			double RowHeightRatio, float BlockHeight,
			double MaxRowWidth
		)
		{
			this->Rows.
				SetData(RowHeightRatio, BlockHeight, MaxRowWidth);

			this->BlockFactory.Emplace(BlockHeight);
		}
	
		void SetSentences(const TArray<FString>& NewSentences)
		{
			this->Sentences = NewSentences;
		}
	
		template<typename UserClass>
		void SetSelectRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnSelectRequested.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetMoveRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*, const FVector&)
		)
		{
			this->OnMoveRequested.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetToggleRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnToggleRequested.BindUObject(Listener, Callback);
		}
	
		template<typename UserClass>
		void SetDropRequestedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnDropRequested.BindUObject(Listener, Callback);
		}

		void CreateBlocks(const TArray<FToken>& Tokens)
		{
			UWorld* World = this->GetWorld();

			TArray<ABlock*> NewBlocks =
				this->BlockFactory->
					CreateBlocks(World, Tokens, this->Sentences);

			this->AddBlocks(NewBlocks);
		}

		void CreateBlock(const FToken& Token)
		{
			ABlock* Block = this->SpawnBlock(Token);

			this->AddBlock(Block);
		}

		void CreateBlockBefore(
			const FToken& Token, ABlock* NextBlock
		)
		{
			ABlock* Block = this->SpawnBlock(Token);

			this->AddBlockBefore(Block, NextBlock);
		}
	
		void RemoveBlock(ABlock* Block)
		{
			Block->Destroy();

			this->Blocks.RemoveSingle(Block);

			this->UpdateLayout();
		}

		void RemoveAllBlocks()
		{
			for (ABlock* Block : this->Blocks)
			{
				Block->Destroy();
			}

			this->Blocks.Reset();

			this->UpdateLayout();
		}
	
		void SetAllMinimized(bool bMinimized)
		{
			for (ABlock* Block : this->Blocks)
			{
				APropositionBlock* Proposition =
					Cast<APropositionBlock>(Block);

				bool bIsProposition = Proposition != nullptr;

				if (bIsProposition)
				{
					Proposition->SetMinimized(bMinimized);
				}
			}
		}
	
		void ToggleMinimized(ABlock* Block)
		{
			APropositionBlock* Proposition =
				CastChecked<APropositionBlock>(Block);

			Proposition->ToggleMinimized();
		}

		TArray<FToken> GetTokens() const
		{
			TArray<FToken> Tokens;

			for (const ABlock* Block : this->Blocks)
			{
				FToken Token = Block->GetToken();

				Tokens.Add(Token);
			}

			return Tokens;
		}
	
		TArray<FBlockState> GetStates() const
		{
			TArray<FBlockState> States;

			for (const ABlock* Block : this->Blocks)
			{
				FBlockState State = Block->GetState();

				States.Add(State);
			}

			return States;
		}

		void ApplyStates(const TArray<FBlockState>& States)
		{
			int32 BlockCount = this->Blocks.Num();

			int32 StateCount = States.Num();

			bool bHasSameCount = BlockCount == StateCount;

			check(bHasSameCount);

			for (int32 Index=0; Index<BlockCount; Index++)
			{
				ABlock* Block = this->Blocks[Index];

				const FBlockState& State = States[Index];

				Block->ApplyState(State);
			}
		}

		void LayoutBlocks()
		{
			this->Rows.LayoutBlocks(this->Blocks);
		}

		void DragBlock(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
			this->Rows.
				DragBlock(this->Blocks, Block, DesiredLocation);
		}

	#pragma endregion
	
	#pragma region PrivateMethods
	private:

		void AddBlocks(const TArray<ABlock*>& NewBlocks)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->RegisterBlock(Block);

				this->Blocks.Add(Block);
			}

			this->UpdateLayout();
		}

		void AddBlock(ABlock* Block)
		{
			this->RegisterBlock(Block);

			this->Blocks.Add(Block);

			this->UpdateLayout();
		}

		void AddBlockBefore(ABlock* Block, ABlock* NextBlock)
		{
			this->RegisterBlock(Block);

			int32 Index = this->Blocks.Find(NextBlock);

			this->Blocks.Insert(Block, Index);

			this->UpdateLayout();
		}
	
		ABlock* SpawnBlock(const FToken& Token)
		{
			UWorld* World = this->GetWorld();

			ABlock* Block =
				this->BlockFactory->
					CreateBlock(World, Token, this->Sentences);

			return Block;
		}
	
		void RegisterBlock(ABlock* Block)
		{
			AActor* Manager = this->GetTypedOuter<AActor>();

			Block->SetOwner(Manager);
			
			Block->
				SetWidthChangedListener(
					this, &UBlockBoard::OnBlockWidthChanged
				);

			Block->
				SetSelectRequestedListener(
					this, &UBlockBoard::OnBlockSelectRequested
				);

			Block->
				SetMoveRequestedListener(
					this, &UBlockBoard::OnBlockMoveRequested
				);

			Block->
				SetDropRequestedListener(
					this, &UBlockBoard::OnBlockDropRequested
				);
			
			APropositionBlock* Proposition =
				Cast<APropositionBlock>(Block);

			bool bIsProposition = Proposition != nullptr;

			if (bIsProposition)
			{
				this->RegisterProposition(Proposition);
			}
		}
	
		void RegisterProposition(APropositionBlock* Proposition)
		{
			Proposition->
				SetToggleMinimizedRequestedListener(
					this,
					&UBlockBoard::OnPropositionToggleRequested
				);
		}

		void OnBlockWidthChanged(ABlock* Block)
		{
			this->UpdateLayout();
		}

		void OnBlockSelectRequested(ABlock* Block)
		{
			bool bHasSelectListener =
				this->OnSelectRequested.IsBound();

			check(bHasSelectListener);

			this->OnSelectRequested.Execute(Block);
		}

		void OnBlockMoveRequested(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
			bool bHasMoveListener =
				this->OnMoveRequested.IsBound();

			check(bHasMoveListener);

			this->OnMoveRequested.Execute(Block, DesiredLocation);
		}

		void OnBlockDropRequested(ABlock* Block)
		{
			bool bHasDropListener =
				this->OnDropRequested.IsBound();

			check(bHasDropListener);

			this->OnDropRequested.Execute(Block);
		}
	
		void OnPropositionToggleRequested(
			APropositionBlock* Proposition
		)
		{
			bool bHasToggleListener =
				this->OnToggleRequested.IsBound();

			check(bHasToggleListener);

			this->OnToggleRequested.Execute(Proposition);
		}
	
		void UpdateLayout()
		{
			this->Rows.UpdateLayout(this->Blocks);
		}

	#pragma endregion

	#pragma region Attributes

		FOnBlockSelectRequested OnSelectRequested;

		FOnBlockMoveRequested OnMoveRequested;

		FOnBlockDropRequested OnDropRequested;
	
		FOnBoardToggleRequested OnToggleRequested;
	
		UPROPERTY()
		TArray<TObjectPtr<ABlock>> Blocks;
	
		TArray<FString> Sentences;

		TOptional<FBlockFactory> BlockFactory;

		FBlockBoardRows Rows;

	#pragma endregion
};