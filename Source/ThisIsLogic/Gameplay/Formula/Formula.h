#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"

#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "Formula.generated.h"

UCLASS()
class THISISLOGIC_API AFormula : public AActor
{
	GENERATED_BODY()

	public:

		AFormula()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName RootName = TEXT("Root");

			USceneComponent* Root =
				CreateDefaultSubobject<
					USceneComponent
				>(RootName);

			this->RootComponent = Root;
		}

		void AddBlock(ABlock* Block)
		{
			this->Blocks.Add(Block);

			Block->SetOwner(this);

			Block->
				AddWidthChangedListener(
					this, &AFormula::OnBlockWidthChanged
				);
		}

		void MoveBlock(ABlock* Block, double DesiredY)
		{
			double MinY = this->GetMinY(Block);
			double MaxY = this->GetMaxY(Block);

			double ClampedY =
				FMath::Clamp(DesiredY, MinY, MaxY);

			FVector Location = Block->GetActorLocation();

			Location.Y = ClampedY;

			Block->SetActorLocation(Location);
		}

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			for (ABlock* Block : this->InitialBlocks)
			{
				bool bHasBlock = Block != nullptr;

				if (bHasBlock)
				{
					this->AddBlock(Block);
				}
			}
		}

	private:

		double GetMinY(const ABlock* Block) const
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			double HalfLength = this->Length * 0.5;

			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			double FormulaLocationY = FormulaLocation.Y;

			return 
				FormulaLocationY - HalfLength + HalfWidth;
		}

		double GetMaxY(const ABlock* Block) const
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			double HalfLength = this->Length * 0.5;

			double HalfWidth = Block->GetWidth() * 0.5;

			double FormulaLocationY = FormulaLocation.Y;

			return
				FormulaLocationY + HalfLength - HalfWidth;
		}

		void OnBlockWidthChanged()
		{
			UE_LOGFMT(
				LogTemp, Warning, "A block width changed"
			);
		}

		UPROPERTY(EditAnywhere, Category = "Formula")
		float Length = 1000.f;

		UPROPERTY(EditAnywhere, Category = "Formula")
		TArray<TObjectPtr<ABlock>> InitialBlocks;

		UPROPERTY()
		TArray<TObjectPtr<ABlock>> Blocks;
};
