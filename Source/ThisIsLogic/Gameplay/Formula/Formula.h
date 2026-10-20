#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../Block/BlockData.h"
#include "../../Logic/LogicTypes.h"
#include "../Block/PropositionBlock.h"
#include "../Block/AbstractBlocksManager.h"

#include "Logging/StructuredLog.h"

#include "Formula.generated.h"

DECLARE_DELEGATE_TwoParams(FOnFormulaSelectionChanged, bool, bool);

DECLARE_DELEGATE(FOnFormulaChanged);

UCLASS()
class THISISLOGIC_API AFormula : public AAbstractBlocksManager
{
	GENERATED_BODY()
	
	#pragma region PublicMethods
	public:

		void SetTokens(const TArray<FToken>& Tokens)
		{
			this->ClearSelection();

			this->RemoveAllBlocks();

			this->CreateBlocks(Tokens);

			this->LogTokens();
		}

		void SetStates(const TArray<FBlockState>& States)
		{
			TArray<FToken> Tokens =
				AFormula::GetStateTokens(States);

			this->SetTokens(Tokens);

			this->ApplyStates(States);
		}

		template<typename UserClass>
		void SetSelectionChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(bool, bool)
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
			this->SetPropositionsMinimized(bMinimized);

			this->NotifyChanged();
		}

		void ToggleSelectedMinimized()
		{
			APropositionBlock* Proposition =
				CastChecked<APropositionBlock>(this->SelectedBlock);

			Proposition->ToggleMinimized();

			this->NotifyChanged();
		}

		void InsertBlock(const FToken& Token)
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				ABlock* Selected = this->SelectedBlock;

				this->CreateBlockBefore(Token, Selected);
			}
			else
			{
				this->CreateBlock(Token);
			}

			this->NotifyChanged();

			this->LogTokens();
		}

		void RemoveSelectedBlock()
		{
			ABlock* Block = this->SelectedBlock;

			this->DeselectBlock();

			this->RemoveBlock(Block);

			this->NotifySelectionChanged();

			this->NotifyChanged();

			this->LogTokens();
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

	#pragma region ProtectedMethods
	protected:

		virtual double GetRowHeightRatio() const override
		{
			return AFormula::RowHeightRatio;
		}

		virtual void RegisterBlock(ABlock* Block) override
		{
			Super::RegisterBlock(Block);

			APropositionBlock* Proposition =
				Cast<APropositionBlock>(Block);

			bool bIsProposition = Proposition != nullptr;

			if (bIsProposition)
			{
				this->RegisterProposition(Proposition);
			}
		}

		virtual void OnBlockSelectRequested(ABlock* Block)
			override
		{
			this->SelectBlock(Block);
		}

		virtual void OnBlockMoveRequested(
			ABlock* Block, const FVector& DesiredLocation
		) override
		{
			this->DragBlock(Block, DesiredLocation);

			this->LogTokens();
		}

		virtual void OnBlockDropRequested(ABlock* Block)
			override
		{
			this->LayoutBlocks();

			this->NotifyChanged();

			this->LogTokens();
		}

	#pragma endregion
	
	#pragma region PrivateMethods
	private:

		static TArray<FToken> GetStateTokens(
			const TArray<FBlockState>& States
		)
		{
			TArray<FToken> Tokens;

			for (const FBlockState& State : States)
			{
				FToken Token = State.Token;

				Tokens.Add(Token);
			}

			return Tokens;
		}
	
		void RegisterProposition(
			APropositionBlock* Proposition
		)
		{
			Proposition->
				SetToggleMinimizedRequestedListener(
					this,
					&AFormula::OnPropositionToggleMinimizedRequested
				);
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

			bool bHasSelection = this->HasSelection();

			this->OnSelectionChanged.
				Execute(
					bHasSelection, this->bIsPropositionSelected
				);
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
		TObjectPtr<ABlock> SelectedBlock;
	
		FOnFormulaSelectionChanged OnSelectionChanged;
	
		FOnFormulaChanged OnChanged;
	
		bool bIsPropositionSelected = false;
	
		static constexpr double RowHeightRatio = 1.5;
	
	#pragma endregion
};