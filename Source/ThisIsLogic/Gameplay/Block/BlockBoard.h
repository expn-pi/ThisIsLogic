#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Block.h"
#include "BlockData.h"
#include "BlockFactory.h"
#include "BlockBoardRows.h"
#include "PropositionBlock.h"
#include "../../Logic/LogicTypes.h"
#include "../../Visuals/RoundedBackgroundComponent.h"
#include "../../BasicComponents/ThisIsLogicSettings.h"

#include "BlockBoard.generated.h"

UCLASS()
class THISISLOGIC_API UBlockBoard : public USceneComponent
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:
	
		UBlockBoard()
		{
			FName UBackGroundName = TEXT("Background");
			
			Background =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(UBackGroundName);
			
			Background->SetupAttachment(this);
		}
	
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
			
			this->BuildBackground(10.f, 10.f);
		}
	
		void SetSentences(
			const TArray<FString>& NewSentences
		)
		{
			this->Sentences = NewSentences;
		}
	
		template<typename UserClass>
		void SetTappedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnTapped.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetLongPressedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnLongPressed.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetDragStartedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnDragStarted.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetDraggedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*, const FVector&)
		)
		{
			this->OnDragged.BindUObject(Listener, Callback);
		}

		template<typename UserClass>
		void SetDragEndedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(ABlock*)
		)
		{
			this->OnDragEnded.BindUObject(Listener, Callback);
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
			int32 Index = this->Blocks.Num();

			this->CreateBlockAt(Token, Index);
		}

		void CreateBlockBefore(
			const FToken& Token, ABlock* NextBlock
		)
		{
			int32 Index = this->Blocks.Find(NextBlock);

			this->CreateBlockAt(Token, Index);
		}

		void CreateBlockAfter(
			const FToken& Token, ABlock* PreviousBlock
		)
		{
			int32 PreviousIndex =
				this->Blocks.Find(PreviousBlock);

			int32 Index = PreviousIndex + 1;

			this->CreateBlockAt(Token, Index);
		}

		void CreateBlockAtStart(const FToken& Token)
		{
			this->CreateBlockAt(Token, 0);
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
		
		void BuildBackground(float Width, float Height)
		{
			this->Background->Build(Width, Height);
			
			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UMaterialInterface* BlockMaterial =
				Settings->GetBlockMaterial();

			this->Background->SetBaseMaterial(BlockMaterial);

			FColor FBackgroundColor = FColor(0xFFFFFFFF);
				
			FLinearColor BackgroundColor =
				FLinearColor(FBackgroundColor);
				
			this->Background->SetColor(BackgroundColor);
		}
	
		void AddBlocks(const TArray<ABlock*>& NewBlocks)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->RegisterBlock(Block);

				this->Blocks.Add(Block);
			}

			this->UpdateLayout();
		}

		void CreateBlockAt(
			const FToken& Token, int32 Index
		)
		{
			ABlock* Block = this->SpawnBlock(Token);

			this->RegisterBlock(Block);

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
				SetTappedListener(
					this, &UBlockBoard::OnBlockTapped
				);

			Block->
				SetLongPressedListener(
					this, &UBlockBoard::OnBlockLongPressed
				);

			Block->
				SetDragStartedListener(
					this, &UBlockBoard::OnBlockDragStarted
				);

			Block->
				SetDraggedListener(
					this, &UBlockBoard::OnBlockDragged
				);

			Block->
				SetDragEndedListener(
					this, &UBlockBoard::OnBlockDragEnded
				);
		}

		void OnBlockWidthChanged(ABlock* Block)
		{
			this->UpdateLayout();
		}

		void OnBlockTapped(ABlock* Block)
		{
			bool bHasTappedListener = this->OnTapped.IsBound();

			check(bHasTappedListener);

			this->OnTapped.Execute(Block);
		}

		void OnBlockLongPressed(ABlock* Block)
		{
			bool bHasLongPressedListener =
				this->OnLongPressed.IsBound();

			check(bHasLongPressedListener);

			this->OnLongPressed.Execute(Block);
		}

		void OnBlockDragStarted(ABlock* Block)
		{
			bool bHasDragStartedListener =
				this->OnDragStarted.IsBound();

			check(bHasDragStartedListener);

			this->OnDragStarted.Execute(Block);
		}

		void OnBlockDragged(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
			bool bHasDraggedListener = this->OnDragged.IsBound();

			check(bHasDraggedListener);

			this->OnDragged.Execute(Block, DesiredLocation);
		}

		void OnBlockDragEnded(ABlock* Block)
		{
			bool bHasDragEndedListener =
				this->OnDragEnded.IsBound();

			check(bHasDragEndedListener);

			this->OnDragEnded.Execute(Block);
		}
	
		void UpdateLayout()
		{
			this->Rows.UpdateLayout(this->Blocks);
		}

	#pragma endregion

	#pragma region Attributes
	
		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Background;

		FOnBlockTapped OnTapped;

		FOnBlockLongPressed OnLongPressed;

		FOnBlockDragStarted OnDragStarted;

		FOnBlockDragged OnDragged;

		FOnBlockDragEnded OnDragEnded;
	
		UPROPERTY()
		TArray<TObjectPtr<ABlock>> Blocks;
	
		TArray<FString> Sentences;

		TOptional<FBlockFactory> BlockFactory;

		FBlockBoardRows Rows;

	#pragma endregion
};