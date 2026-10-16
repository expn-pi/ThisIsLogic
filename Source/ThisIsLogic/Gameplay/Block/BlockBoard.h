#pragma once

#include "CoreMinimal.h"

#include "Block.h"
#include "BlockRow.h"

#include "BlockBoard.generated.h"

USTRUCT()
struct FBlockBoard
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		void SetOrigin(const FVector& NewOrigin)
		{
			this->Origin = NewOrigin;
		}

		void SetBlockHeight(float BlockHeight)
		{
			this->RowHeight =
				BlockHeight * FBlockBoard::RowHeightRatio;

			this->DragHeight =
				BlockHeight * FBlockBoard::DragHeightRatio;
		}

		void SetMaxRowWidth(double NewMaxRowWidth)
		{
			this->MaxRowWidth = NewMaxRowWidth;
		}

		void AddBlocks(
			const TArray<ABlock*>& NewBlocks
		)
		{
			for (ABlock* Block : NewBlocks)
			{
				this->Blocks.Add(Block);
			}

			this->UpdateLayout();
		}

		void AddBlock(ABlock* Block)
		{
			this->Blocks.Add(Block);

			this->UpdateLayout();
		}
	
		void RemoveBlock(ABlock* Block)
		{
			this->Blocks.RemoveSingle(Block);

			this->UpdateLayout();
		}

		void RemoveAllBlocks()
		{
			this->Blocks.Reset();

			this->UpdateLayout();
		}
	
		const TArray<TObjectPtr<ABlock>>& GetBlocks()
			const
		{
			return this->Blocks;
		}

		void UpdateLayout()
		{
			this->UpdateRows();

			this->LayoutBlocks();
		}

		void LayoutBlocks()
		{
			for (const FBlockRow& Row : this->Rows)
			{
				Row.LayoutBlocks(this->Blocks);
			}
		}

		void DragBlock(
			ABlock* Block,
			const FVector& DesiredLocation
		)
		{
			FVector Location =
				this->ClampToArea(Block, DesiredLocation);

			double CenterX = Location.X;

			double CenterY = Location.Y;

			int32 BlockRowIndex =
				this->GetRowIndexOf(Block);

			int32 TargetRowIndex =
				this->GetRowIndexAt(CenterX);

			FBlockRow TargetRow =
				this->Rows[TargetRowIndex];

			bool bIsSameRow =
				BlockRowIndex == TargetRowIndex;

			if (bIsSameRow)
			{
				this->ReorderBlock(
					Block, TargetRow, CenterY
				);
			}
			else
			{
				this->EnterRow(
					Block, TargetRow, CenterY
				);
			}

			this->MoveBlock(Block, Location);
		}

	#pragma endregion

	#pragma region PrivateMethods
	private:

		void UpdateRows()
		{
			this->Rows.Reset();

			int32 BlockCount = this->Blocks.Num();

			for (int32 Index=0; Index<BlockCount; Index++)
			{
				this->PlaceInRows(Index);
			}
		}

		void PlaceInRows(int32 Index)
		{
			const ABlock* Block = this->Blocks[Index];

			double BlockWidth = Block->GetWidth();

			bool bFits = this->FitsInLastRow(BlockWidth);

			if (bFits)
			{
				this->ExtendLastRow(Index, BlockWidth);
			}
			else
			{
				this->AddRow(Index, BlockWidth);
			}
		}

		bool FitsInLastRow(double BlockWidth) const
		{
			bool bFits = false;

			bool bHasRow = !this->Rows.IsEmpty();

			if (bHasRow)
			{
				const FBlockRow& LastRow =
					this->Rows.Last();

				bFits = LastRow.Fits(BlockWidth);
			}

			return bFits;
		}

		void ExtendLastRow(
			int32 Index, double BlockWidth
		)
		{
			FBlockRow& LastRow = this->Rows.Last();

			LastRow.AddBlock(Index, BlockWidth);
		}

		void AddRow(int32 Index, double BlockWidth)
		{
			int32 RowIndex = this->Rows.Num();

			FVector Start = this->Origin;

			Start.X = this->GetRowCenterX(RowIndex);

			Start.Y = this->GetLeftEdgeY();

			FBlockRow Row =
				FBlockRow(
					Index, BlockWidth,
					Start, this->MaxRowWidth
				);

			this->Rows.Add(Row);
		}

		double GetRowCenterX(int32 RowIndex) const
		{
			double RowOffset = this->RowHeight * RowIndex;

			double OriginX = this->Origin.X;

			return OriginX - RowOffset;
		}

		double GetLeftEdgeY() const
		{
			double HalfMaxRowWidth =
				this->MaxRowWidth * 0.5;

			double OriginY = this->Origin.Y;

			return OriginY - HalfMaxRowWidth;
		}

		double GetRightEdgeY() const
		{
			double HalfMaxRowWidth =
				this->MaxRowWidth * 0.5;

			double OriginY = this->Origin.Y;

			return OriginY + HalfMaxRowWidth;
		}

		FVector ClampToArea(
			const ABlock* Block,
			const FVector& DesiredLocation
		) const
		{
			double DesiredX = DesiredLocation.X;

			double DesiredY = DesiredLocation.Y;

			FVector Location = DesiredLocation;

			Location.X = this->ClampX(DesiredX);

			Location.Y = this->ClampY(Block, DesiredY);

			return Location;
		}

		double ClampX(double DesiredX) const
		{
			int32 RowCount = this->Rows.Num();

			int32 LastRowIndex = RowCount - 1;

			double MaxX = this->GetRowCenterX(0);

			double MinX =
				this->GetRowCenterX(LastRowIndex);

			return FMath::Clamp(DesiredX, MinX, MaxX);
		}

		double ClampY(
			const ABlock* Block, double DesiredY
		) const
		{
			double MinY = this->GetMinY(Block);
			double MaxY = this->GetMaxY(Block);

			return FMath::Clamp(DesiredY, MinY, MaxY);
		}

		double GetMinY(const ABlock* Block) const
		{
			double LeftEdgeY = this->GetLeftEdgeY();

			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			return LeftEdgeY + HalfWidth;
		}

		double GetMaxY(const ABlock* Block) const
		{
			double RightEdgeY = this->GetRightEdgeY();

			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			return RightEdgeY - HalfWidth;
		}

		int32 GetRowIndexOf(ABlock* Block) const
		{
			int32 BlockIndex = this->Blocks.Find(Block);

			int32 FoundRowIndex = 0;

			int32 RowCount = this->Rows.Num();

			for (
				int32 RowIndex=0;
				RowIndex<RowCount;
				RowIndex++
			)
			{
				const FBlockRow& Row =
					this->Rows[RowIndex];

				bool bContains = Row.Contains(BlockIndex);

				if (bContains)
				{
					FoundRowIndex = RowIndex;
				}
			}

			return FoundRowIndex;
		}

		int32 GetRowIndexAt(double CenterX) const
		{
			double OriginX = this->Origin.X;

			double Offset = OriginX - CenterX;

			double RowPosition = Offset / this->RowHeight;

			return FMath::RoundToInt32(RowPosition);
		}

		void EnterRow(
			ABlock* Block, const FBlockRow& Row,
			double CenterY
		)
		{
			int32 Index = this->Blocks.Find(Block);

			int32 EntryIndex =
				Row.GetEntryIndex(this->Blocks, CenterY);

			bool bComesFromBefore = Index < EntryIndex;

			if (bComesFromBefore)
			{
				EntryIndex = EntryIndex - 1;
			}

			bool bMoves = EntryIndex != Index;

			if (bMoves)
			{
				this->Blocks.RemoveAt(Index);

				this->Blocks.Insert(Block, EntryIndex);

				this->UpdateLayout();
			}
		}

		void ReorderBlock(
			ABlock* Block, const FBlockRow& Row,
			double CenterY
		)
		{
			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			double LeftEdgeY = CenterY - HalfWidth;

			double RightEdgeY = CenterY + HalfWidth;

			int32 Index = this->Blocks.Find(Block);

			Index =
				this->ReorderIncreasing(
					Row, Index, RightEdgeY
				);

			this->ReorderDecreasing(Row, Index, LeftEdgeY);
		}

		int32 ReorderIncreasing(
			const FBlockRow& Row, int32 Index,
			double RightEdgeY
		)
		{
			bool bIsPastNext =
				Row.IsPastNextBlock(
					this->Blocks, Index, RightEdgeY
				);

			while (bIsPastNext)
			{
				int32 NextIndex = Index + 1;

				this->Blocks.
					Swap(
						Index,
						NextIndex
					);

				Row.LayoutBlocks(this->Blocks);

				Index = NextIndex;

				bIsPastNext =
					Row.IsPastNextBlock(
						this->Blocks, Index, RightEdgeY
					);
			}

			return Index;
		}

		void ReorderDecreasing(
			const FBlockRow& Row, int32 Index,
			double LeftEdgeY
		)
		{
			bool bIsPastPrevious =
				Row.IsPastPreviousBlock(
					this->Blocks, Index, LeftEdgeY
				);

			while (bIsPastPrevious)
			{
				int32 PreviousIndex = Index - 1;

				this->Blocks.
					Swap(
						Index,
						PreviousIndex
					);

				Row.LayoutBlocks(this->Blocks);

				Index = PreviousIndex;

				bIsPastPrevious =
					Row.IsPastPreviousBlock(
						this->Blocks, Index, LeftEdgeY
					);
			}
		}

		void MoveBlock(
			ABlock* Block, const FVector& Location
		)
		{
			FVector LiftedLocation = Location;

			double OriginZ = this->Origin.Z;

			LiftedLocation.Z =
				OriginZ + this->DragHeight;

			Block->SetActorLocation(LiftedLocation);
		}

	#pragma endregion

	#pragma region Attributes

		UPROPERTY()
		TArray<TObjectPtr<ABlock>> Blocks;

		TArray<FBlockRow> Rows;

		FVector Origin = FVector::ZeroVector;

		double MaxRowWidth = 0.0;

		double RowHeight = 0.0;

		double DragHeight = 0.0;

		static constexpr double RowHeightRatio = 1.5;

		static constexpr double DragHeightRatio = 0.2;

	#pragma endregion
};