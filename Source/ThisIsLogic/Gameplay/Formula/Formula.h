#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../Block/BlockData.h"
#include "../../Logic/LogicTypes.h"
#include "../Block/AbstractBlocksManager.h"

#include "Formula.generated.h"

DECLARE_DELEGATE_ThreeParams(
	FOnFormulaSelectionChanged, bool, bool, bool
);

DECLARE_DELEGATE(FOnFormulaChanged);

UCLASS()
class THISISLOGIC_API AFormula : public AAbstractBlocksManager
{
	GENERATED_BODY()
	
	#pragma region PublicMethods
	public:

		void SetSentences(
			const TArray<FString>& Sentences
		)
		{
			this->Board->SetSentences(Sentences);
		}
	
		void SetTokens(const TArray<FToken>& Tokens)
		{
			this->ClearSelection();

			this->Board->RemoveAllBlocks();

			this->Board->CreateBlocks(Tokens);
		}

		void SetStates(const TArray<FBlockState>& States)
		{
			TArray<FToken> Tokens =
				AFormula::GetStateTokens(States);

			this->SetTokens(Tokens);

			this->Board->ApplyStates(States);
		}

		TArray<FBlockState> GetStates() const
		{
			return this->Board->GetStates();
		}
	
		TArray<FToken> GetTokens() const
		{
			return this->Board->GetTokens();
		}
	
		template<typename UserClass>
		void SetSelectionChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)(bool, bool, bool)
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
			this->Board->SetAllMinimized(bMinimized);

			this->NotifySelectionChanged();

			this->NotifyChanged();
		}

		void ToggleSelectedMinimized()
		{
			ABlock* Selected = this->SelectedBlock;

			this->Board->ToggleMinimized(Selected);

			this->NotifySelectionChanged();

			this->NotifyChanged();
		}

		void InsertBlock(const FToken& Token)
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				ABlock* Selected = this->SelectedBlock;

				ABlock* NewBlock =
					this->Board->
						CreateBlockAfter(Token, Selected);

				this->SelectBlock(NewBlock);
			}
			else
			{
				this->Board->CreateBlockAtStart(Token);
			}

			this->NotifyChanged();
		}
	
		void InsertBlockBefore(const FToken& Token)
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				ABlock* Selected = this->SelectedBlock;

				ABlock* NewBlock =
					this->Board->
						CreateBlockBefore(Token, Selected);

				this->SelectBlock(NewBlock);
			}
			else
			{
				this->Board->CreateBlockAtEnd(Token);
			}

			this->NotifyChanged();
		}
	
		void RemoveSelectedBlock()
		{
			ABlock* Selected = this->SelectedBlock;

			ABlock* NextBlock =
				this->Board->GetNextBlock(Selected);

			bool bHasNext = NextBlock != nullptr;

			if (bHasNext)
			{
				this->SelectBlock(NextBlock);
			}
			else
			{
				this->ClearSelection();
			}

			this->Board->RemoveBlock(Selected);

			this->NotifyChanged();
		}

	#pragma endregion

	#pragma region ProtectedMethods
	protected:

		virtual double GetRowHeightRatio() const override
		{
			return AFormula::RowHeightRatio;
		}
	
		virtual void OnBlockTapped(ABlock* Block)
			override
		{
			this->SelectBlock(Block);
		}

		virtual void OnBlockLongPressed(ABlock* Block)
			override
		{
			this->SelectBlock(Block);

			if (this->bIsPropositionSelected)
			{
				this->ToggleSelectedMinimized();
			}
		}

		virtual void OnBlockDragStarted(ABlock* Block)
			override
		{
			this->SelectBlock(Block);
		}

		virtual void OnBlockDragged(
			ABlock* Block, const FVector& DesiredLocation
		)
			override
		{
			this->Board->DragBlock(Block, DesiredLocation);
		}

		virtual void OnBlockDragEnded(ABlock* Block)
			override
		{
			this->Board->LayoutBlocks();

			this->NotifyChanged();
		}
	
		virtual int32 GetBackgroundRowCount() const override
		{
			return AFormula::BackgroundRowCount;
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
	
		void SelectBlock(ABlock* Block)
		{
			this->DeselectBlock();

			this->SelectedBlock = Block;

			this->SelectedBlock->SetSelected(true);

			FToken Token = Block->GetToken();

			ETokenKind Kind = Token.Kind;

			this->bIsPropositionSelected =
				Kind == ETokenKind::Proposition;

			this->NotifySelectionChanged();
		}

		void DeselectBlock()
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				this->SelectedBlock->SetSelected(false);

				this->SelectedBlock = nullptr;
				
				this->bIsPropositionSelected = false;
			}
		}
		
		bool HasSelection() const
		{
			return this->SelectedBlock != nullptr;
		}

		bool IsSelectedMinimized() const
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				FBlockState State =
					this->SelectedBlock->GetState();

				return State.bMinimized;
			}
			else
			{
				return false;
			}
		}
	
		void NotifySelectionChanged()
		{
			bool bHasSelectionListener =
				this->OnSelectionChanged.IsBound();

			check(bHasSelectionListener);

			bool bHasSelection = this->HasSelection();

			bool bIsSelectedMinimized =
				this->IsSelectedMinimized();

			this->OnSelectionChanged.
				Execute(
					bHasSelection, this->bIsPropositionSelected,
					bIsSelectedMinimized
				);
		}
	
		void NotifyChanged()
		{
			bool bHasChangedListener = this->OnChanged.IsBound();

			check(bHasChangedListener);

			this->OnChanged.Execute();
		}
	
	#pragma endregion
	
	#pragma region Attributes
	
		UPROPERTY()
		TObjectPtr<ABlock> SelectedBlock;
	
		FOnFormulaSelectionChanged OnSelectionChanged;
	
		FOnFormulaChanged OnChanged;
	
		bool bIsPropositionSelected = false;
	
		static constexpr double RowHeightRatio = 1.5;
	
		static constexpr int32 BackgroundRowCount = 6;
	
	#pragma endregion
};