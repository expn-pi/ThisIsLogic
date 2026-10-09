#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

#include "../Block/Block.h"
#include "../Formula/Formula.h"
#include "../Block/BlockFactory.h"
#include "../../Logic/FormulaStub.h"
#include "../../Input/BackgroundTarget.h"

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
		}

		void CreateBlocks()
		{
			UWorld* World = this->GetWorld();

			FFormulaStub FormulaStub;

			TArray<FToken> Tokens =
				FormulaStub.GetTokens();

			TArray<ABlock*> Blocks;

			for (const FToken& Token : Tokens)
			{
				ABlock* Block =
					FBlockFactory::
						CreateBlock(World, Token);

				Blocks.Add(Block);
			}

			this->Formula->AddBlocks(Blocks);
		}

		void OnBackgroundTapped()
		{
			this->Formula->ClearSelection();
		}

		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		static constexpr double BackgroundDepth = 100.0;
};