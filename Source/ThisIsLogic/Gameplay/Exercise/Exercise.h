#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"

#include "../Block/Block.h"
#include "../Level/LevelRow.h"
#include "../Formula/Formula.h"
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
				CreateDefaultSubobject<
					USceneComponent
				>(RootName);

			this->RootComponent = Root;
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			this->LoadLevel();

			this->CreateBackground();

			this->CreateFormula();

			this->CreateBlocks();

			this->CreatePanel();
		}

	#pragma region PrivateMethods
	private:

		void LoadLevel()
		{
			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UDataTable* LevelTable =
				Settings->GetLevelTable();

			TArray<FName> RowNames =
				LevelTable->GetRowNames();

			FName FirstRowName = RowNames[0];

			const TCHAR* ContextString = TEXT("AExercise");

			const FLevelRow* Row =
				LevelTable->
					FindRow<
						FLevelRow
					>(FirstRowName, ContextString);

			bool bHasRow = Row != nullptr;

			check(bHasRow);

			this->LevelRow = *Row;
		}

		void CreateBackground()
		{
			UWorld* World = this->GetWorld();

			FVector Location = this->GetActorLocation();

			double ExerciseLocationZ = Location.Z;

			Location.Z =
				ExerciseLocationZ -
				AExercise::BackgroundDepth;

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
					SpawnActor<
						AFormula
					>(
						FormulaClass, Transform
					);

			this->Formula->SetOwner(this);
			
			this->Formula->
				SetBlockHeight(AExercise::BlockHeight);

			this->Formula->
				SetSelectionChangedListener(
					this,
					&AExercise::
						OnFormulaSelectionChanged
				);
		}

		void CreateBlocks()
		{
			UWorld* World = this->GetWorld();
			
			// For quick tests, change below:
			//		AnalyzeFormula with
			//			FormulaStub.GetTokens	
			//		And this->LevelRow.Sentences with
			//			FormulaStub.GetSentences();
			
			//FFormulaStub FormulaStub;
			
			TArray<FToken> Tokens =
				this->AnalyzeFormula();
				//FormulaStub.GetTokens();

			const TArray<FString>& Sentences =
				this->LevelRow.Sentences;
				//FormulaStub.GetSentences();

			TArray<ABlock*> Blocks =
				this->BlockFactory.
					CreateBlocks(World, Tokens, Sentences);

			this->Formula->AddBlocks(Blocks);
		}

		TArray<FToken> AnalyzeFormula() const
		{
			const TCHAR* FormulaChars =
				*this->LevelRow.Formula;

			std::string FormulaText =
				TCHAR_TO_UTF8(FormulaChars);

			std::optional<std::vector<FToken>> AnalyzedTokens =
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
					this,
					&AExercise::OnAllMinimizedRequested
				);

			this->CommandPanel->
				SetToggleMinimizedRequestedListener(
					this,
					&AExercise::OnToggleMinimizedRequested
				);

			this->UpdatePanel();

			this->CommandPanel->AddToViewport();
		}

		void UpdatePanel()
		{
			bool bIsPropositionSelected =
				this->Formula->IsPropositionSelected();

			this->CommandPanel->
				SetToggleMinimizedVisible(
					bIsPropositionSelected
				);
		}

		void OnBackgroundTapped()
		{
			this->Formula->ClearSelection();
		}

		void OnFormulaSelectionChanged()
		{
			this->UpdatePanel();
		}

		void OnAllMinimizedRequested(bool bMinimized)
		{
			this->Formula->SetAllMinimized(bMinimized);
		}

		void OnToggleMinimizedRequested()
		{
			this->Formula->ToggleSelectedMinimized();
		}

	#pragma endregion
	
	#pragma region Attributes
	
		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		UPROPERTY()
		TObjectPtr<UCommandPanel> CommandPanel;

		FLevelRow LevelRow;

		FBlockFactory BlockFactory = 
			FBlockFactory('P', AExercise::BlockHeight);

		static constexpr float BlockHeight = 2.f;

		static constexpr double BackgroundDepth = 5.0;
	
	#pragma endregion
};