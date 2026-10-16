#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../Block/BlockBoard.h"
#include "../Block/PropositionBlock.h"

#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "Formula.generated.h"

DECLARE_DELEGATE(FOnFormulaSelectionChanged);

UCLASS()
class THISISLOGIC_API AFormula : public AActor
{
	GENERATED_BODY()
	
	#pragma region PublicMethods
	public:

		AFormula()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName RootName = TEXT("Root");

			USceneComponent* Root =
				CreateDefaultSubobject<
					USceneComponent
				>(RootName);

			this->RootComponent = Root;
		}
		
		void SetBlockHeight(float BlockHeight)
		{
			this->Board.SetBlockHeight(BlockHeight);
		}
	
		void SetMaxRowWidth(double MaxRowWidth)
		{
			this->Board.SetMaxRowWidth(MaxRowWidth);
		}
	
		void AddBlocks(
			const TArray<ABlock*>& NewBlocks
		)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->RegisterBlock(Block);
			}

			this->Board.AddBlocks(NewBlocks);
		}

		void AddBlock(ABlock* Block)
		{
			this->RegisterBlock(Block);

			this->Board.AddBlock(Block);
		}
	
		template<typename UserClass>
		void SetSelectionChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnSelectionChanged.
				BindUObject(Listener, Callback);
		}

		void ClearSelection()
		{
			this->DeselectBlock();

			this->NotifySelectionChanged();
		}

		void SetAllMinimized(bool bMinimized)
		{
			const TArray<TObjectPtr<ABlock>>& Blocks =
				this->Board.GetBlocks();

			for (ABlock* Block : Blocks)
			{
				APropositionBlock* Proposition =
					Cast<APropositionBlock>(Block);

				bool bIsProposition =
					Proposition != nullptr;

				if (bIsProposition)
				{
					Proposition->SetMinimized(bMinimized);
				}
			}
		}

		void ToggleSelectedMinimized()
		{
			APropositionBlock* Proposition =
				CastChecked<
					APropositionBlock
				>(this->SelectedBlock);

			Proposition->ToggleMinimized();
		}
	
		void RemoveSelectedBlock()
		{
			ABlock* Block = this->SelectedBlock;

			this->DeselectBlock();

			this->Board.RemoveBlock(Block);

			Block->Destroy();

			this->NotifySelectionChanged();
		}
	
		void RemoveAllBlocks()
		{
			TArray<TObjectPtr<ABlock>> RemovedBlocks =
				this->Board.GetBlocks();

			this->DeselectBlock();

			this->Board.RemoveAllBlocks();

			for (ABlock* Block : RemovedBlocks)
			{
				Block->Destroy();
			}

			this->NotifySelectionChanged();
		}

		bool HasSelection() const
		{
			return this->SelectedBlock != nullptr;
		}
	
		bool IsPropositionSelected() const
		{
			return this->bIsPropositionSelected;
		}
	
	#pragma endregion

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			FVector Location = this->GetActorLocation();

			this->Board.SetOrigin(Location);
		}
	
	#pragma region PrivateMethods
	private:

		void RegisterBlock(ABlock* Block)
		{
			Block->SetOwner(this);

			Block->
				SetWidthChangedListener(
					this, &AFormula::OnBlockWidthChanged
				);

			Block->
				SetMoveRequestedListener(
					this, &AFormula::OnBlockMoveRequested
				);

			Block->
				SetDropRequestedListener(
					this, &AFormula::OnBlockDropRequested
				);

			Block->
				SetSelectRequestedListener(
					this, &AFormula::OnBlockSelectRequested
				);

			APropositionBlock* Proposition =
				Cast<APropositionBlock>(Block);

			bool bIsProposition = Proposition != nullptr;

			if (bIsProposition)
			{
				this->RegisterProposition(Proposition);
			}
		}

		void RegisterProposition(
			APropositionBlock* Proposition
		)
		{
			Proposition->
				SetToggleMinimizedRequestedListener(
					this,
					&AFormula::
						OnPropositionToggleMinimizedRequested
				);
		}

		void OnBlockMoveRequested(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
			this->Board.DragBlock(Block, DesiredLocation);
		}

		void OnBlockDropRequested(ABlock* Block)
		{
			this->Board.LayoutBlocks();
		}

		void OnBlockWidthChanged(ABlock* Block)
		{
			this->Board.UpdateLayout();
		}

		void OnBlockSelectRequested(ABlock* Block)
		{
			this->SelectBlock(Block);
		}

		void SelectBlock(ABlock* Block)
		{
			this->DeselectBlock();

			this->SelectedBlock = Block;

			this->SelectedBlock->SetSelected(true);

			APropositionBlock* Proposition =
				Cast<APropositionBlock>(Block);

			this->bIsPropositionSelected =
				Proposition != nullptr;

			this->NotifySelectionChanged();
		}

		void DeselectBlock()
		{
			bool bHasSelection =
				this->SelectedBlock != nullptr;

			if (bHasSelection)
			{
				this->SelectedBlock->SetSelected(false);

				this->SelectedBlock = nullptr;
				
				this->bIsPropositionSelected = false;
			}
		}

		void NotifySelectionChanged()
		{
			bool bHasSelectionListener =
				this->OnSelectionChanged.IsBound();

			check(bHasSelectionListener);

			this->OnSelectionChanged.Execute();
		}
	
		void OnPropositionToggleMinimizedRequested(
			APropositionBlock* Proposition
		)
		{
			ABlock* Selected = this->SelectedBlock;

			bool bIsSelected = Proposition == Selected;

			check(bIsSelected);

			this->ToggleSelectedMinimized();
		}

	#pragma endregion
	
	#pragma region Attributes
	
		UPROPERTY()
		FBlockBoard Board;

		UPROPERTY()
		TObjectPtr<ABlock> SelectedBlock;

		FOnFormulaSelectionChanged OnSelectionChanged;

		bool bIsPropositionSelected = false;
	
	#pragma endregion
};