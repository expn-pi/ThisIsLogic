#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../Block/BlockData.h"
#include "../Block/BlockBoard.h"
#include "../../Logic/LogicTypes.h"
#include "../Block/PropositionBlock.h"

#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "Formula.generated.h"

DECLARE_DELEGATE(FOnFormulaSelectionChanged);

DECLARE_DELEGATE(FOnFormulaChanged);

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
				CreateDefaultSubobject<USceneComponent>(RootName);

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
	
		void AddBlocks(const TArray<ABlock*>& NewBlocks)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->RegisterBlock(Block);
			}

			this->Board.AddBlocks(NewBlocks);
			
			this->LogTokens();
		}

		void AddBlock(ABlock* Block)
		{
			this->RegisterBlock(Block);

			this->Board.AddBlock(Block);
		}
	
		template<typename UserClass>
		void SetSelectionChangedListener(
			UserClass* Listener, void (UserClass::* Callback)()
		)
		{
			this->OnSelectionChanged.
				BindUObject(Listener, Callback);
		}
	
		template<typename UserClass>
		void SetChangedListener(
			UserClass* Listener, void (UserClass::* Callback)()
		)
		{
			this->OnChanged.BindUObject(Listener, Callback);
		}

		void ClearSelection()
		{
			this->DeselectBlock();

			this->NotifySelectionChanged();
		}

		void SetAllMinimized(bool bMinimized)
		{
			this->Board.SetAllMinimized(bMinimized);
			
			this->NotifyChanged();
		}

		void ToggleSelectedMinimized()
		{
			APropositionBlock* Proposition =
				CastChecked<APropositionBlock>(this->SelectedBlock);

			Proposition->ToggleMinimized();
			
			this->NotifyChanged();
		}
	
		void RemoveSelectedBlock()
		{
			ABlock* Block = this->SelectedBlock;

			this->DeselectBlock();

			this->Board.RemoveBlock(Block);

			this->NotifySelectionChanged();
			
			this->NotifyChanged();
			
			this->LogTokens();
		}
	
		void RemoveAllBlocks()
		{
			this->DeselectBlock();

			this->Board.RemoveAllBlocks();

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
	
		TArray<FToken> GetTokens() const
		{
			return this->Board.GetTokens();
		}
	
		TArray<FBlockState> GetStates() const
		{
			return this->Board.GetStates();
		}

		void ApplyStates(const TArray<FBlockState>& States)
		{
			this->Board.ApplyStates(States);
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
			
			this->LogTokens();
		}

		void OnBlockDropRequested(ABlock* Block)
		{
			this->Board.LayoutBlocks();
			
			this->NotifyChanged();
			
			this->LogTokens();
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

			this->bIsPropositionSelected = Proposition != nullptr;

			this->NotifySelectionChanged();
		}

		void DeselectBlock()
		{
			bool bHasSelection = this->SelectedBlock != nullptr;

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
	
		void NotifyChanged()
		{
			bool bHasChangedListener = this->OnChanged.IsBound();

			check(bHasChangedListener);

			this->OnChanged.Execute();
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

		void LogTokens() const
		{
			TArray<FToken> Tokens = this->GetTokens();

			UE_LOGFMT(LogTemp, Warning, "Tokens:");

			for (const FToken& Token : Tokens)
			{
				ETokenKind Kind = Token.Kind;
				
				int32 KindIndex = static_cast<int32>(Kind);

				int32 Index = Token.Index;

				UE_LOGFMT(
					LogTemp, Warning,
					"Kind {0}, index {1}", KindIndex, Index
				);
			}
		}
	
	#pragma endregion
	
	#pragma region Attributes
	
		UPROPERTY()
		FBlockBoard Board;

		UPROPERTY()
		TObjectPtr<ABlock> SelectedBlock;

		FOnFormulaSelectionChanged OnSelectionChanged;
	
		FOnFormulaChanged OnChanged;

		bool bIsPropositionSelected = false;
	
	#pragma endregion
};