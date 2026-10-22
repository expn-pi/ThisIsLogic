#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"
#include "../Block/BlockData.h"
#include "../../Logic/LogicTypes.h"
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

		void SetSentences(const TArray<FString>& Sentences)
		{
			this->Board->SetSentences(Sentences);
		}
	
		void SetTokens(const TArray<FToken>& Tokens)
		{
			this->ClearSelection();

			this->Board->RemoveAllBlocks();

			this->Board->CreateBlocks(Tokens);

			this->LogTokens();
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
			this->Board->SetAllMinimized(bMinimized);

			this->NotifyChanged();
		}

		void ToggleSelectedMinimized()
		{
			ABlock* Selected = this->SelectedBlock;

			this->Board->ToggleMinimized(Selected);

			this->NotifyChanged();
		}

		void InsertBlock(const FToken& Token)
		{
			bool bHasSelection = this->HasSelection();

			if (bHasSelection)
			{
				ABlock* Selected = this->SelectedBlock;

				this->Board->CreateBlockBefore(Token, Selected);
			}
			else
			{
				this->Board->CreateBlock(Token);
			}

			this->NotifyChanged();

			this->LogTokens();
		}

		void RemoveSelectedBlock()
		{
			ABlock* Block = this->SelectedBlock;

			this->DeselectBlock();

			this->Board->RemoveBlock(Block);

			this->NotifySelectionChanged();

			this->NotifyChanged();

			this->LogTokens();
		}

	#pragma endregion

	#pragma region ProtectedMethods
	protected:

		virtual double GetRowHeightRatio() const override
		{
			return AFormula::RowHeightRatio;
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
			this->Board->DragBlock(Block, DesiredLocation);

			this->LogTokens();
		}

		virtual void OnBlockDropRequested(ABlock* Block)
			override
		{
			this->Board->LayoutBlocks();

			this->NotifyChanged();

			this->LogTokens();
		}
	
		virtual void OnBlockToggleRequested(ABlock* Block)
			override
		{
			ABlock* Selected = this->SelectedBlock;

			bool bIsSelected = Block == Selected;

			check(bIsSelected);

			this->ToggleSelectedMinimized();
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

		void LogTokens() const
		{
			TArray<FToken> Tokens = this->Board->GetTokens();

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