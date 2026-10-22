#pragma once

#include "CoreMinimal.h"

#include "Block.h"
#include "BlockRow.h"

class FBlockBoardRows
{
	#pragma region PublicMethods
	public:

		void SetOrigin(const FVector& NewOrigin)
		{
			this->Origin = NewOrigin;
		}

		void SetData(
			double RowHeightRatio, float BlockHeight,
			double NewMaxRowWidth
		)
		{
			this->RowHeight = BlockHeight * RowHeightRatio;

			this->DragHeight =
				BlockHeight * FBlockBoardRows::DragHeightRatio;

			this->MaxRowWidth = NewMaxRowWidth;
		}

		void UpdateLayout(
			const TArray<TObjectPtr<ABlock>>& Blocks
		)
		{
			this->UpdateRows(Blocks);

			this->LayoutBlocks(Blocks);
		}

		void LayoutBlocks(
			const TArray<TObjectPtr<ABlock>>& Blocks
		) const
		{
			for (const FBlockRow& Row : this->Rows)
			{
				Row.LayoutBlocks(Blocks);
			}
		}
	
		void DragBlock(
			TArray<TObjectPtr<ABlock>>& Blocks, ABlock* Block,
			const FVector& DesiredLocation
		)
		{
			FVector Location =
				this->ClampToArea(Block, DesiredLocation);

			double CenterX = Location.X;

			double CenterY = Location.Y;

			int32 BlockRowIndex =
				this->GetRowIndexOf(Blocks, Block);

			int32 TargetRowIndex = this->GetRowIndexAt(CenterX);

			FBlockRow TargetRow = this->Rows[TargetRowIndex];

			bool bIsSameRow = BlockRowIndex == TargetRowIndex;

			if (bIsSameRow)
			{
				this->ReorderBlock(
					Blocks, Block, TargetRow, CenterY
				);
			}
			else
			{
				this->EnterRow(
					Blocks, Block, TargetRow, CenterY
				);
			}

			this->MoveBlock(Block, Location);
		}

	#pragma endregion

	#pragma region PrivateMethods
	private:

		void UpdateRows(
			const TArray<TObjectPtr<ABlock>>& Blocks
		)
		{
			this->Rows.Reset();

			int32 BlockCount = Blocks.Num();

			for (int32 Index=0; Index<BlockCount; Index++)
			{
				this->PlaceInRows(Blocks, Index);
			}
		}

		void PlaceInRows(
			const TArray<TObjectPtr<ABlock>>& Blocks, int32 Index
		)
		{
			const ABlock* Block = Blocks[Index];

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

			bool bIsEmpty = this->Rows.IsEmpty();
			
			bool bHasRow = !bIsEmpty;

			if (bHasRow)
			{
				const FBlockRow& LastRow = this->Rows.Last();

				bFits = LastRow.Fits(BlockWidth);
			}

			return bFits;
		}

		void ExtendLastRow(int32 Index, double BlockWidth)
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
			double HalfMaxRowWidth = this->MaxRowWidth * 0.5;

			double OriginY = this->Origin.Y;

			return OriginY - HalfMaxRowWidth;
		}

		double GetRightEdgeY() const
		{
			double HalfMaxRowWidth = this->MaxRowWidth * 0.5;

			double OriginY = this->Origin.Y;

			return OriginY + HalfMaxRowWidth;
		}

		FVector ClampToArea(
			const ABlock* Block, const FVector& DesiredLocation
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

			double MinX = this->GetRowCenterX(LastRowIndex);

			return FMath::Clamp(DesiredX, MinX, MaxX);
		}

		double ClampY(const ABlock* Block, double DesiredY)
			const
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

		int32 GetRowIndexOf(
			const TArray<TObjectPtr<ABlock>>& Blocks, ABlock* Block
		) const
		{
			int32 BlockIndex = Blocks.Find(Block);

			int32 FoundRowIndex = 0;

			int32 RowCount = this->Rows.Num();

			for (int32 RowIndex=0; RowIndex<RowCount; RowIndex++)
			{
				const FBlockRow& Row = this->Rows[RowIndex];

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
			TArray<TObjectPtr<ABlock>>& Blocks, ABlock* Block,
			const FBlockRow& Row, double CenterY
		)
		{
			int32 Index = Blocks.Find(Block);

			int32 EntryIndex = Row.GetEntryIndex(Blocks, CenterY);

			bool bComesFromBefore = Index < EntryIndex;

			if (bComesFromBefore)
			{
				EntryIndex = EntryIndex - 1;
			}

			bool bMoves = EntryIndex != Index;

			if (bMoves)
			{
				Blocks.RemoveAt(Index);

				Blocks.Insert(Block, EntryIndex);

				this->UpdateLayout(Blocks);
			}
		}
	
		void ReorderBlock(
			TArray<TObjectPtr<ABlock>>& Blocks, ABlock* Block,
			const FBlockRow& Row, double CenterY
		)
		{
			double BlockWidth = Block->GetWidth();

			double HalfWidth = BlockWidth * 0.5;

			double LeftEdgeY = CenterY - HalfWidth;

			double RightEdgeY = CenterY + HalfWidth;

			int32 Index = Blocks.Find(Block);

			Index =
				this->ReorderIncreasing(
					Blocks, Row, Index, RightEdgeY
				);

			this->ReorderDecreasing(
				Blocks, Row, Index, LeftEdgeY
			);
		}

		int32 ReorderIncreasing(
			TArray<TObjectPtr<ABlock>>& Blocks,
			const FBlockRow& Row, int32 Index, double RightEdgeY
		)
		{
			bool bIsPastNext =
				Row.IsPastNextBlock(
					Blocks, Index, RightEdgeY
				);

			while (bIsPastNext)
			{
				int32 NextIndex = Index + 1;

				Blocks.
					Swap(
						Index,
						NextIndex
					);

				Row.LayoutBlocks(Blocks);

				Index = NextIndex;

				bIsPastNext =
					Row.IsPastNextBlock(
						Blocks, Index, RightEdgeY
					);
			}

			return Index;
		}

		void ReorderDecreasing(
			TArray<TObjectPtr<ABlock>>& Blocks,
			const FBlockRow& Row, int32 Index, double LeftEdgeY
		)
		{
			bool bIsPastPrevious =
				Row.IsPastPreviousBlock(
					Blocks, Index, LeftEdgeY
				);

			while (bIsPastPrevious)
			{
				int32 PreviousIndex = Index - 1;

				Blocks.
					Swap(
						Index,
						PreviousIndex
					);

				Row.LayoutBlocks(Blocks);

				Index = PreviousIndex;

				bIsPastPrevious =
					Row.IsPastPreviousBlock(
						Blocks, Index, LeftEdgeY
					);
			}
		}

		void MoveBlock(
			ABlock* Block, const FVector& Location
		)
		{
			FVector LiftedLocation = Location;

			double OriginZ = this->Origin.Z;

			LiftedLocation.Z = OriginZ + this->DragHeight;

			Block->SetActorLocation(LiftedLocation);
		}
	
	#pragma endregion

	#pragma region Attributes

		TArray<FBlockRow> Rows;

		FVector Origin = FVector::ZeroVector;

		double MaxRowWidth = 0.0;

		double RowHeight = 0.0;

		double DragHeight = 0.0;

		static constexpr double DragHeightRatio = 0.2;

	#pragma endregion
};