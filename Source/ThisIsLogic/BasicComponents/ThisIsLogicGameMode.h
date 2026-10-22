#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"

#include "CameraPawn.h"
#include "../UI/CommandPanel.h"
#include "ThisIsLogicSettings.h"
#include "../Input/BackgroundTarget.h"
#include "../Gameplay/Formula/Formula.h"
#include "../Gameplay/Exercise/Exercise.h"
#include "../Gameplay/Palette/OperatorPalette.h"

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

			AFormula* Formula = this->CreateFormula();

			this->CreateBackground(Formula);

			AExercise* Exercise = this->SpawnExercise(Formula);

			this->CreatePanel(Formula, Exercise);

			this->StartExercise(Exercise);

			this->CreateOperatorPalette(Formula);
		}

	#pragma region PrivateMethods
	private:

		AFormula* CreateFormula()
		{
			UWorld* World = this->GetWorld();

			FVector Location = FVector::ZeroVector;

			FTransform Transform = FTransform(Location);

			UClass* FormulaClass = AFormula::StaticClass();

			AFormula* Formula =
				World->
					SpawnActor<AFormula>(FormulaClass, Transform);

			Formula->
				SetMaxRowWidth(AThisIsLogicGameMode::MaxRowWidth);

			return Formula;
		}

		void CreateBackground(AFormula* Formula)
		{
			UWorld* World = this->GetWorld();

			double LocationZ =
				-AThisIsLogicGameMode::BackgroundDepth;

			FVector Location = FVector(0.0, 0.0, LocationZ);

			FTransform Transform = FTransform(Location);

			UClass* BackgroundClass =
				ABackgroundTarget::StaticClass();

			ABackgroundTarget* Background =
				World->
					SpawnActor<
						ABackgroundTarget
					>(BackgroundClass, Transform);

			Background->
				SetTappedListener(
					Formula, &AFormula::ClearSelection
				);
		}

		AExercise* SpawnExercise(AFormula* Formula)
		{
			UWorld* World = this->GetWorld();

			FTransform Transform = FTransform::Identity;

			UClass* ExerciseClass = AExercise::StaticClass();

			AExercise* Exercise =
				World->
					SpawnActorDeferred<
						AExercise
					>(ExerciseClass, Transform);

			Exercise->SetFormula(Formula);

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

			Panel->AddToViewport();
		}

		void StartExercise(AExercise* Exercise)
		{
			FTransform Transform = FTransform::Identity;

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
	
	#pragma endregion

		static constexpr double MaxRowWidth = 40.0;

		static constexpr double BackgroundDepth = 5.0;
};