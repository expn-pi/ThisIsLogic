#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"

#include "CameraPawn.h"
#include "../UI/CommandPanel.h"
#include "../UI/ResultScreen.h"
#include "ThisIsLogicSettings.h"
#include "../Input/BackgroundTarget.h"
#include "../Gameplay/Level/LevelRow.h"
#include "../Gameplay/Formula/Formula.h"
#include "../Gameplay/Exercise/Exercise.h"
//#include "../Gameplay/Level/QuickTestLevel.h"
#include "../Gameplay/Palette/OperatorPalette.h"
#include "../Gameplay/Palette/PropositionPalette.h"

#include "ThisIsLogicGameMode.generated.h"

UCLASS()
class THISISLOGIC_API AThisIsLogicGameMode :
	public AGameModeBase
{
	GENERATED_BODY()

	public:

		AThisIsLogicGameMode()
		{
			this->DefaultPawnClass = ACameraPawn::StaticClass();
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();
			
			FLevelRow LevelRow = this->ReadLevelRow();

			// For quick tests, change below:
			//		LevelRow.Formula with
			//			FQuickTestLevel::GetFormula();
			//		LevelRow.Solution with
			//			FQuickTestLevel::GetSolution();
			//		And LevelRow.Sentences with
			//			FQuickTestLevel::GetSentences();

			FString StartingText =
				LevelRow.Formula;
				//FQuickTestLevel::GetFormula();

			FString SolutionText =
				LevelRow.Solution;
				//FQuickTestLevel::GetSolution();

			TArray<FString> Sentences =
				LevelRow.Sentences;
				//FQuickTestLevel::GetSentences();

			AFormula* Formula = this->CreateFormula(Sentences);

			this->CreateBackgroundTarget(Formula);

			AExercise* Exercise =
				this->SpawnExercise(
					Formula, StartingText, SolutionText
				);

			this->CreatePanel(Formula, Exercise);
			
			this->CreateResultScreen(Exercise);
			
			this->StartExercise(Exercise);

			this->CreateOperatorPalette(Formula);

			this->CreatePropositionPalette(Formula, Sentences);
		}

	#pragma region PrivateMethods
	private:
	
		FLevelRow ReadLevelRow() const
		{
			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UDataTable* LevelTable = Settings->GetLevelTable();

			TArray<FName> RowNames = LevelTable->GetRowNames();

			FName FirstRowName = RowNames[0];

			const TCHAR* ContextString =
				TEXT("AThisIsLogicGameMode");

			const FLevelRow* Row =
				LevelTable->
					FindRow<FLevelRow>(FirstRowName, ContextString);

			bool bHasRow = Row != nullptr;

			check(bHasRow);

			return *Row;
		}

		AFormula* CreateFormula(
			const TArray<FString>& Sentences
		)
		{
			UWorld* World = this->GetWorld();

			FVector Location = FVector(1.6, 0.0, 0.0);

			FTransform Transform = FTransform(Location);

			UClass* FormulaClass = AFormula::StaticClass();

			AFormula* Formula =
				World->
					SpawnActor<AFormula>(FormulaClass, Transform);

			Formula->
				SetMaxRowWidth(AThisIsLogicGameMode::MaxRowWidth);

			Formula->SetSentences(Sentences);

			return Formula;
		}

		void CreateBackgroundTarget(AFormula* Formula)
		{
			UWorld* World = this->GetWorld();

			double LocationZ =
				-AThisIsLogicGameMode::BackgroundTargetDepth;

			FVector Location =
				FVector(0.0, 0.0, LocationZ);

			FTransform Transform = FTransform(Location);

			UClass* TargetClass =
				ABackgroundTarget::StaticClass();

			ABackgroundTarget* Target =
				World->
					SpawnActor<
						ABackgroundTarget
					>(TargetClass, Transform);

			Target->
				SetTappedListener(
					Formula, &AFormula::ClearSelection
				);
		}

		FTransform GetExerciseTransform() const
		{
			FVector Location = FVector(5.1, 20.6, 0.0);

			FTransform Transform = FTransform(Location);

			return Transform;
		}

		AExercise* SpawnExercise(
			AFormula* Formula, const FString& StartingText,
			const FString& SolutionText
		)
		{
			UWorld* World = this->GetWorld();

			FTransform Transform = this->GetExerciseTransform();

			UClass* ExerciseClass = AExercise::StaticClass();

			AExercise* Exercise =
				World->
					SpawnActorDeferred<
						AExercise
					>(ExerciseClass, Transform);

			Exercise->SetFormula(Formula);

			Exercise->
				SetFormulasTexts(
					StartingText, SolutionText
				);

			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			UMaterialInterface* Material =
				Settings->GetBlockMaterial();

			UFont* Font = Settings->GetBlockFont();

			UMaterialInterface* TextMaterial =
				Settings->GetTextMaterial();

			Exercise->
				SetIndicatorsAssets(
					Material, Font, TextMaterial
				);

			return Exercise;
		}

		void CreatePanel(
			AFormula* Formula, AExercise* Exercise
		)
		{
			UWorld* World = this->GetWorld();

			APlayerController* PlayerController =
				World->GetFirstPlayerController();

			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			TSubclassOf<UCommandPanel> PanelClass =
				Settings->GetCommandPanelClass();

			UCommandPanel* Panel =
				CreateWidget<
					UCommandPanel
				>(PlayerController, PanelClass);

			Formula->
				SetSelectionChangedListener(
					Panel, &UCommandPanel::UpdateSelection
				);

			Exercise->
				SetEquivalenceChangedListener(
					Panel, &UCommandPanel::UpdateEquivalence
				);

			Panel->
				SetAllMinimizedRequestedListener(
					Formula, &AFormula::SetAllMinimized
				);

			Panel->
				SetToggleMinimizedRequestedListener(
					Formula, &AFormula::ToggleSelectedMinimized
				);

			Panel->
				SetRemoveRequestedListener(
					Formula, &AFormula::RemoveSelectedBlock
				);

			Panel->
				SetRestartRequestedListener(
					Exercise, &AExercise::Restart
				);

			Panel->
				SetUndoRequestedListener(
					Exercise, &AExercise::Undo
				);

			Panel->
				SetSubmitRequestedListener(
					Exercise, &AExercise::Submit
				);

			Panel->AddToViewport();
		}

		void CreateResultScreen(AExercise* Exercise)
		{
			UWorld* World = this->GetWorld();

			APlayerController* PlayerController =
				World->GetFirstPlayerController();

			const UThisIsLogicSettings* Settings =
				GetDefault<UThisIsLogicSettings>();

			TSubclassOf<UResultScreen> ScreenClass =
				Settings->GetResultScreenClass();

			UResultScreen* Screen =
				CreateWidget<
					UResultScreen
				>(PlayerController, ScreenClass);

			Exercise->
				SetSubmittedListener(
					Screen, &UResultScreen::Show
				);

			Screen->
				SetOkRequestedListener(
					Exercise, &AExercise::Restart
				);

			int32 ZOrder =
				AThisIsLogicGameMode::ResultScreenZOrder;

			Screen->AddToViewport(ZOrder);
		}
	
		void StartExercise(AExercise* Exercise)
		{
			FTransform Transform = this->GetExerciseTransform();

			Exercise->FinishSpawning(Transform);
		}

		void CreateOperatorPalette(AFormula* Formula)
		{
			UWorld* World = this->GetWorld();

			FVector Location =
				FVector(13.0, -22.5, 0.0);

			FTransform Transform = FTransform(Location);

			UClass* PaletteClass =
				AOperatorPalette::StaticClass();

			AOperatorPalette* Palette =
				World->
					SpawnActor<
						AOperatorPalette
					>(PaletteClass, Transform);

			Palette->
				SetInsertRequestedListener(
					Formula, &AFormula::InsertBlock
				);
			
			Palette->
				SetInsertAfterRequestedListener(
					Formula, &AFormula::InsertBlockAfter
				);
		}
	
		void CreatePropositionPalette(
			AFormula* Formula, const TArray<FString>& Sentences
		)
		{
			UWorld* World = this->GetWorld();

			FVector Location = FVector(13.0, 0.0, 0.0);

			FTransform Transform = FTransform(Location);

			UClass* PaletteClass =
				APropositionPalette::StaticClass();

			APropositionPalette* Palette =
				World->
					SpawnActorDeferred<
						APropositionPalette
					>(PaletteClass, Transform);

			Palette->SetSentences(Sentences);

			Palette->
				SetInsertRequestedListener(
					Formula, &AFormula::InsertBlock
				);

			Palette->
				SetInsertAfterRequestedListener(
					Formula, &AFormula::InsertBlockAfter
				);

			Palette->FinishSpawning(Transform);
		}
	
	#pragma endregion

		static constexpr double MaxRowWidth = 40.0;

		static constexpr double BackgroundTargetDepth = 5.0;
	
		static constexpr int32 ResultScreenZOrder = 1;
};