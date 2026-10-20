#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "../Block/Block.h"
#include "../Block/BlockBoard.h"
#include "../Block/BlockFactory.h"
#include "../../Logic/LogicTypes.h"

#include "Palette.generated.h"

UCLASS(Abstract)
class THISISLOGIC_API APalette : public AActor
{
	GENERATED_BODY()

	public:

		APalette()
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

			FVector Location = this->GetActorLocation();

			this->Board.SetOrigin(Location);

			double MaxRowWidth = this->GetMaxRowWidth();

			this->Board.
				SetData(
					APalette::RowHeightRatio,
					APalette::BlockHeight, MaxRowWidth
				);

			this->CreateBlocks();
		}

		virtual TArray<FToken> GetTokens() const
		{
			unimplemented();

			return TArray<FToken>();
		}

		virtual double GetMaxRowWidth() const
		{
			unimplemented();

			return 0.0;
		}

		static constexpr float BlockHeight = 2.f;

	private:

		void CreateBlocks()
		{
			UWorld* World = this->GetWorld();

			TArray<FToken> Tokens = this->GetTokens();

			TArray<FString> Sentences;

			TArray<ABlock*> Blocks =
				this->BlockFactory.
					CreateBlocks(World, Tokens, Sentences);

			for (ABlock* Block : Blocks)
			{
				this->RegisterBlock(Block);
			}

			this->Board.AddBlocks(Blocks);
		}

		void RegisterBlock(ABlock* Block)
		{
			Block->SetOwner(this);

			Block->
				SetWidthChangedListener(
					this, &APalette::OnBlockWidthChanged
				);

			Block->
				SetMoveRequestedListener(
					this, &APalette::OnBlockMoveRequested
				);

			Block->
				SetDropRequestedListener(
					this, &APalette::OnBlockDropRequested
				);

			Block->
				SetSelectRequestedListener(
					this, &APalette::OnBlockSelectRequested
				);
		}

		void OnBlockWidthChanged(ABlock* Block)
		{
			this->Board.UpdateLayout();
		}

		void OnBlockMoveRequested(
			ABlock* Block, const FVector& DesiredLocation
		)
		{
		}

		void OnBlockDropRequested(ABlock* Block)
		{
		}

		void OnBlockSelectRequested(ABlock* Block)
		{
			FToken Token = Block->GetToken();

			ETokenKind Kind = Token.Kind;

			int32 KindIndex = static_cast<int32>(Kind);

			UE_LOGFMT(
				LogTemp, Warning,
				"Palette tapped: kind {0}", KindIndex
			);
		}

		UPROPERTY()
		FBlockBoard Board;

		FBlockFactory BlockFactory =
			FBlockFactory(APalette::BlockHeight);
	
		static constexpr double RowHeightRatio = 1.1;
};