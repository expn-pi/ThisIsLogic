#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"
#include "GameFramework/PlayerController.h"

#include "../Block/Block.h"
#include "../Formula/Formula.h"
#include "../Block/BlockFactory.h"
#include "../../UI/CommandPanel.h"
#include "../../Logic/FormulaStub.h"
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

			this->CreateBackground();

			this->CreateFormula();

			this->CreateBlocks();
			
			this->CreatePanel();
		}

	private:

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
				SetSelectionChangedListener(
					this,
					&AExercise::
						OnFormulaSelectionChanged
				);
		}

		void CreateBlocks()
		{
			UWorld* World = this->GetWorld();

			FFormulaStub FormulaStub;

			TArray<FToken> Tokens =
				FormulaStub.GetTokens();

			TArray<FString> Sentences =
				FormulaStub.GetSentences();

			TArray<ABlock*> Blocks;

			for (const FToken& Token : Tokens)
			{
				ABlock* Block =
					this->BlockFactory.
						CreateBlock(World, Token, Sentences);

				Blocks.Add(Block);
			}

			this->Formula->AddBlocks(Blocks);
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

		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		UPROPERTY()
		TObjectPtr<UCommandPanel> CommandPanel;

		FBlockFactory BlockFactory = FBlockFactory('P');

		static constexpr double BackgroundDepth = 100.0;
};