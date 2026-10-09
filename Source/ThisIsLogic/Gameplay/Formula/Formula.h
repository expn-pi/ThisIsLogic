#pragma once

#include "CoreMinimal.h"

#include "../Block/Block.h"

#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "Formula.generated.h"

DECLARE_DELEGATE(FOnFormulaSelectionChanged);

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

		void AddBlocks(
			const TArray<ABlock*>& NewBlocks
		)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->RegisterBlock(Block);
			}

			this->UpdateRow();
		}

		void AddBlock(ABlock* Block)
		{
			this->RegisterBlock(Block);

			this->UpdateRow();
		}
	
		template<typename UserClass>
		void SetSelectionChangedListener(
			UserClass* Listener,
			void (UserClass::* Callback)()
		)
		{
			this->OnSelectionChanged.
				BindUObject(Listener, Callback);
		}

		void ClearSelection()
		{
			this->DeselectBlock();

			this->NotifySelectionChanged();
		}

	private:

		void RegisterBlock(ABlock* Block)
		{
			this->Blocks.Add(Block);

			Block->SetOwner(this);

			Block->
				SetWidthChangedListener(
					this, &AFormula::OnBlockWidthChanged
				);

			Block->
				SetMoveRequestedListener(
					this, &AFormula::OnBlockMoveRequested
				);

			Block->
				SetDropRequestedListener(
					this, &AFormula::OnBlockDropRequested
				);

			Block->
				SetSelectRequestedListener(
					this, &AFormula::OnBlockSelectRequested
				);
		}

		void UpdateRow()
		{
			this->UpdateTotalWidth();

			this->LayoutBlocks();
		}

		void UpdateTotalWidth()
		{
			double Sum = 0.0;

			for (const ABlock* Block : this->Blocks)
			{
				double BlockWidth = Block->GetWidth();

				Sum = Sum + BlockWidth;
			}

			this->TotalWidth = Sum;
		}

		void LayoutBlocks()
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			double CursorY = this->GetLeftEdgeY();

			for (ABlock* Block : this->Blocks)
			{
				double BlockWidth = Block->GetWidth();

				double HalfWidth = BlockWidth * 0.5;

				FVector Location = FormulaLocation;

				Location.Y = CursorY + HalfWidth;

				Block->SetActorLocation(Location);

				CursorY = CursorY + BlockWidth;
			}
		}

		double GetLeftEdgeY() const
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			double HalfTotalWidth = this->TotalWidth * 0.5;

			double FormulaLocationY = FormulaLocation.Y;

			return FormulaLocationY - HalfTotalWidth;
		}

		double GetRightEdgeY() const
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			double HalfTotalWidth = this->TotalWidth * 0.5;

			double FormulaLocationY = FormulaLocation.Y;

			return FormulaLocationY + HalfTotalWidth;
		}

		void OnBlockMoveRequested(
			ABlock* Block,
			const FVector& DesiredLocation
		)
		{
			double DesiredY = DesiredLocation.Y;

			double CenterY = this->ClampY(Block, DesiredY);

			this->ReorderBlock(Block, CenterY);

			this->MoveBlock(Block, CenterY);
		}

		double ClampY(
			const ABlock* Block, double DesiredY
		) const
		{
			double MinY = this->GetMinY(Block);
			double MaxY = this->GetMaxY(Block);

			return FMath::Clamp(DesiredY, MinY, MaxY);
		}

		double GetMinY(
			const ABlock* Block
		) const
		{
			double LeftEdgeY = this->GetLeftEdgeY();

			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			return LeftEdgeY + HalfWidth;
		}

		double GetMaxY(
			const ABlock* Block
		) const
		{
			double RightEdgeY = this->GetRightEdgeY();

			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			return RightEdgeY - HalfWidth;
		}

		void ReorderBlock(
			ABlock* Block, double CenterY
		)
		{
			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			double LeftEdgeY = CenterY - HalfWidth;

			double RightEdgeY = CenterY + HalfWidth;

			int32 Index = this->Blocks.Find(Block);

			Index =
				this->ReorderIncreasing(
					Index, RightEdgeY
				);

			this->ReorderDecreasing(Index, LeftEdgeY);
		}

		int32 ReorderIncreasing(
			int32 Index, double RightEdgeY
		)
		{
			bool bIsPastNext =
				this->IsPastNextBlock(Index, RightEdgeY);

			while (bIsPastNext)
			{
				int32 NextIndex = Index + 1;

				this->Blocks.Swap(Index, NextIndex);

				this->LayoutBlocks();

				Index = NextIndex;

				bIsPastNext =
					this->IsPastNextBlock(
						Index, RightEdgeY
					);
			}

			return Index;
		}

		void ReorderDecreasing(
			int32 Index, double LeftEdgeY
		)
		{
			bool bIsPastPrevious =
				this->IsPastPreviousBlock(
					Index, LeftEdgeY
				);

			while (bIsPastPrevious)
			{
				int32 PreviousIndex = Index - 1;

				this->Blocks.Swap(Index, PreviousIndex);

				this->LayoutBlocks();

				Index = PreviousIndex;

				bIsPastPrevious =
					this->IsPastPreviousBlock(
						Index, LeftEdgeY
					);
			}
		}

		bool IsPastNextBlock(
			int32 Index, double RightEdgeY
		) const
		{
			bool bIsPast = false;

			int32 NextIndex = Index + 1;

			bool bHasNextBlock =
				this->Blocks.IsValidIndex(NextIndex);

			if (bHasNextBlock)
			{
				const ABlock* NextBlock =
					this->Blocks[NextIndex];

				FVector NextLocation =
					NextBlock->GetActorLocation();

				double NextLocationY = NextLocation.Y;

				bIsPast = RightEdgeY > NextLocationY;
			}

			return bIsPast;
		}

		bool IsPastPreviousBlock(
			int32 Index, double LeftEdgeY
		) const
		{
			bool bIsPast = false;

			int32 PreviousIndex = Index - 1;

			bool bHasPreviousBlock =
				this->Blocks.IsValidIndex(PreviousIndex);

			if (bHasPreviousBlock)
			{
				const ABlock* PreviousBlock =
					this->Blocks[PreviousIndex];

				FVector PreviousLocation =
					PreviousBlock->GetActorLocation();

				bIsPast = LeftEdgeY < PreviousLocation.Y;
			}

			return bIsPast;
		}

		void MoveBlock(
			ABlock* Block, double CenterY
		)
		{
			FVector FormulaLocation =
				this->GetActorLocation();

			FVector Location = FormulaLocation;

			Location.Y = CenterY;

			double FormulaLocationZ = FormulaLocation.Z;

			Location.Z =
				FormulaLocationZ + this->DragHeight;

			Block->SetActorLocation(Location);
		}

		void OnBlockDropRequested(
			ABlock* Block
		)
		{
			this->LayoutBlocks();
		}

		void OnBlockWidthChanged(
			ABlock* Block
		)
		{
			this->UpdateRow();
		}

		void OnBlockSelectRequested(
			ABlock* Block
		)
		{
			this->SelectBlock(Block);
		}

		void SelectBlock(ABlock* Block)
		{
			this->DeselectBlock();

			this->SelectedBlock = Block;

			this->SelectedBlock->SetSelected(true);

			this->NotifySelectionChanged();
		}

		void DeselectBlock()
		{
			bool bHasSelection =
				this->SelectedBlock != nullptr;

			if (bHasSelection)
			{
				this->SelectedBlock->SetSelected(false);

				this->SelectedBlock = nullptr;
			}
		}

		void NotifySelectionChanged()
		{
			bool bHasSelectionListener =
				this->OnSelectionChanged.IsBound();

			check(bHasSelectionListener);

			this->OnSelectionChanged.Execute();
		}

		UPROPERTY()
		TArray<TObjectPtr<ABlock>> Blocks;

		UPROPERTY()
		TObjectPtr<ABlock> SelectedBlock;

		FOnFormulaSelectionChanged OnSelectionChanged;

		double TotalWidth = 0.0;

		static constexpr double DragHeight = 10.0;
};