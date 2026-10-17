#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"

#include "UndoHistory.h"
#include "../Block/Block.h"
#include "../Level/LevelRow.h"
#include "../Formula/Formula.h"
#include "../Block/BlockData.h"
// #include "../Level/FormulaStub.h"
#include "../Block/BlockFactory.h"
#include "../../UI/CommandPanel.h"
#include "../../Logic/FormulaAnalyzer.h"
#include "../../Input/BackgroundTarget.h"
#include "../../BasicComponents/ThisIsLogicSettings.h"

#include "Exercise.generated.h"

UCLASS()
class THISISLOGIC_API AExercise : public AActor
{
	GENERATED_BODY()

	public:

		AExercise()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName RootName = TEXT("Root");

			USceneComponent* Root =
				CreateDefaultSubobject<USceneComponent>(RootName);

			this->RootComponent = Root;
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			this->LoadLevel();

			this->LoadFormula();

			this->CreateBackground();

			this->CreateFormula();

			this->CreateBlocks(this->StartingTokens);
			
			this->SaveState();

			this->CreatePanel();
		}

	#pragma region PrivateMethods
	private:

		void LoadLevel()
		{
			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UDataTable* LevelTable = Settings->GetLevelTable();

			TArray<FName> RowNames = LevelTable->GetRowNames();

			FName FirstRowName = RowNames[0];

			const TCHAR* ContextString = TEXT("AExercise");

			const FLevelRow* Row =
				LevelTable->
					FindRow<FLevelRow>(FirstRowName, ContextString);

			bool bHasRow = Row != nullptr;

			check(bHasRow);

			this->LevelRow = *Row;
		}
	
		void LoadFormula()
		{
			// For quick tests, change below:
			//		AnalyzeFormula with
			//			FormulaStub.GetTokens
			//		And this->LevelRow.Sentences with
			//			FormulaStub.GetSentences();

			//FFormulaStub FormulaStub;

			this->StartingTokens =
				this->AnalyzeFormula();
				//FormulaStub.GetTokens();

			this->Sentences =
				this->LevelRow.Sentences;
				//FormulaStub.GetSentences();
		}

		void CreateBackground()
		{
			UWorld* World = this->GetWorld();

			FVector Location = this->GetActorLocation();

			double ExerciseLocationZ = Location.Z;

			Location.Z =
				ExerciseLocationZ - AExercise::BackgroundDepth;

			FTransform Transform = FTransform(Location);

			UClass* BackgroundClass =
				ABackgroundTarget::StaticClass();

			ABackgroundTarget* BackgroundTarget =
				World->
					SpawnActor<
						ABackgroundTarget
					>(BackgroundClass, Transform);

			BackgroundTarget->SetOwner(this);

			BackgroundTarget->
				SetTappedListener(
					this, &AExercise::OnBackgroundTapped
				);
		}

		void CreateFormula()
		{
			UWorld* World = this->GetWorld();

			FVector Location = this->GetActorLocation();

			FTransform Transform = FTransform(Location);

			UClass* FormulaClass = AFormula::StaticClass();

			this->Formula =
				World->
					SpawnActor<AFormula>(FormulaClass, Transform);

			this->Formula->SetOwner(this);
			
			this->Formula->
				SetBlockHeight(AExercise::BlockHeight);

			this->Formula->
				SetMaxRowWidth(AExercise::MaxRowWidth);

			this->Formula->
				SetSelectionChangedListener(
					this, &AExercise::OnFormulaSelectionChanged
				);
			
			this->Formula->
				SetChangedListener(
					this, &AExercise::OnFormulaChanged
				);
		}

		void CreateBlocks(const TArray<FToken>& Tokens)
		{
			UWorld* World = this->GetWorld();

			TArray<ABlock*> Blocks =
				this->BlockFactory.
					CreateBlocks(World, Tokens, this->Sentences);

			this->Formula->AddBlocks(Blocks);
		}

		TArray<FToken> AnalyzeFormula() const
		{
			const TCHAR* FormulaChars = *this->LevelRow.Formula;

			std::string FormulaText = TCHAR_TO_UTF8(FormulaChars);

			std::optional<std::vector<FToken>>
				AnalyzedTokens =
					FFormulaAnalyzer::Analyze(FormulaText);

			bool bIsValid = AnalyzedTokens.has_value();

			check(bIsValid);

			const std::vector<FToken>& ValidTokens =
				AnalyzedTokens.value();

			const FToken* TokenData = ValidTokens.data();

			std::size_t TokenCount = ValidTokens.size();

			int32 TokenNum = static_cast<int32>(TokenCount);

			TArray<FToken> Tokens =
				TArray<FToken>(TokenData, TokenNum);

			return Tokens;
		}

		void CreatePanel()
		{
			UWorld* World = this->GetWorld();

			APlayerController* PlayerController =
				World->GetFirstPlayerController();

			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			TSubclassOf<UCommandPanel> PanelClass =
				Settings->GetCommandPanelClass();

			this->CommandPanel =
				CreateWidget<
					UCommandPanel
				>(PlayerController, PanelClass);

			this->CommandPanel->
				SetAllMinimizedRequestedListener(
					this, &AExercise::OnAllMinimizedRequested
				);

			this->CommandPanel->
				SetToggleMinimizedRequestedListener(
					this, &AExercise::OnToggleMinimizedRequested
				);
			
			this->CommandPanel->
				SetRemoveRequestedListener(
					this, &AExercise::OnRemoveRequested
				);
			
			this->CommandPanel->
				SetRestartRequestedListener(
					this, &AExercise::OnRestartRequested
				);
			
			this->CommandPanel->
				SetUndoRequestedListener(
					this, &AExercise::OnUndoRequested
				);

			this->UpdatePanel();

			this->CommandPanel->AddToViewport();
		}

		void UpdatePanel()
		{
			bool bHasSelection = this->Formula->HasSelection();

			bool bIsPropositionSelected =
				this->Formula->IsPropositionSelected();

			this->CommandPanel->
				SetSelectionButtonsVisible(bHasSelection);

			this->CommandPanel->
				SetToggleMinimizedVisible(bIsPropositionSelected);
		}
	
		void Restart()
		{
			this->Formula->RemoveAllBlocks();

			this->CreateBlocks(this->StartingTokens);
			
			this->SaveState();
		}
	
		void Undo()
		{
			bool bCanUndo = this->UndoHistory.CanUndo();

			if (bCanUndo)
			{
				TArray<FBlockState> Snapshot =
					this->UndoHistory.Undo();

				TArray<FToken> Tokens =
					AExercise::GetTokens(Snapshot);

				this->Formula->RemoveAllBlocks();

				this->CreateBlocks(Tokens);

				this->Formula->ApplyStates(Snapshot);
			}
		}

		void SaveState()
		{
			TArray<FBlockState> Snapshot =
				this->Formula->GetStates();

			this->UndoHistory.Save(Snapshot);
		}
	
		static TArray<FToken> GetTokens(
			const TArray<FBlockState>& Snapshot
		)
		{
			TArray<FToken> Tokens;

			for (const FBlockState& State : Snapshot)
			{
				FToken Token = State.Token;

				Tokens.Add(Token);
			}

			return Tokens;
		}

		void OnBackgroundTapped()
		{
			this->Formula->ClearSelection();
		}

		void OnFormulaSelectionChanged()
		{
			this->UpdatePanel();
		}
	
		void OnFormulaChanged()
		{
			this->SaveState();
		}

		void OnAllMinimizedRequested(bool bMinimized)
		{
			this->Formula->SetAllMinimized(bMinimized);
		}

		void OnToggleMinimizedRequested()
		{
			this->Formula->ToggleSelectedMinimized();
		}
	
		void OnRemoveRequested()
		{
			this->Formula->RemoveSelectedBlock();
		}
	
		void OnRestartRequested()
		{
			this->Restart();
		}
	
		void OnUndoRequested()
		{
			this->Undo();
		}

	#pragma endregion

	#pragma region Attributes
	
		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		UPROPERTY()
		TObjectPtr<UCommandPanel> CommandPanel;

		FLevelRow LevelRow;
		
		TArray<FToken> StartingTokens;

		TArray<FString> Sentences;
	
		FUndoHistory UndoHistory;
	
		FBlockFactory BlockFactory =
			FBlockFactory(
				'P', ESymbolStyle::Textbook,
				AExercise::BlockHeight
			);

		static constexpr float BlockHeight = 2.f;
	
		static constexpr double MaxRowWidth = 40.0;

		static constexpr double BackgroundDepth = 5.0;
	
	#pragma endregion
};