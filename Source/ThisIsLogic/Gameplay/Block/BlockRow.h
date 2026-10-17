#pragma once

#include "CoreMinimal.h"

#include "Block.h"

class FBlockRow
{
	#pragma region PublicMethods
	public:

		FBlockRow(
			int32 InFirstIndex, double InFirstWidth,
			const FVector& InStart, double InMaxWidth
		) :
			FirstIndex(InFirstIndex),
			LastIndex(InFirstIndex),
			Width(InFirstWidth),
			Start(InStart),
			MaxWidth(InMaxWidth)
		{
		}

		bool Contains(int32 Index) const
		{
			bool bIsFromFirst = Index >= this->FirstIndex;

			bool bIsUpToLast = Index <= this->LastIndex;

			return bIsFromFirst && bIsUpToLast;
		}

		bool Fits(double BlockWidth) const
		{
			double NewWidth = this->Width + BlockWidth;

			return NewWidth <= this->MaxWidth;
		}

		void AddBlock(int32 Index, double BlockWidth)
		{
			this->LastIndex = Index;

			this->Width = this->Width + BlockWidth;
		}

		void LayoutBlocks(
			const TArray<TObjectPtr<ABlock>>& Blocks
		) const
		{
			double CursorY = this->Start.Y;

			for (
				int32 Index=this->FirstIndex;
				Index<=this->LastIndex;
				Index++
			)
			{
				ABlock* Block = Blocks[Index];

				double BlockWidth = Block->GetWidth();

				double HalfWidth = BlockWidth * 0.5;

				FVector Location = this->Start;

				Location.Y = CursorY + HalfWidth;

				Block->SetActorLocation(Location);

				CursorY = CursorY + BlockWidth;
			}
		}

		int32 GetEntryIndex(
			const TArray<TObjectPtr<ABlock>>& Blocks,
			double CenterY
		) const
		{
			int32 EntryIndex = this->FirstIndex;

			for (
				int32 Index=this->FirstIndex;
				Index<=this->LastIndex;
				Index++
			)
			{
				const ABlock* Block = Blocks[Index];

				FVector Location = Block->GetActorLocation();
				
				double LocationY = Location.Y;
				
				bool bIsToTheLeft = LocationY < CenterY;

				if (bIsToTheLeft)
				{
					EntryIndex = Index + 1;
				}
			}

			return EntryIndex;
		}

		bool IsPastNextBlock(
			const TArray<TObjectPtr<ABlock>>& Blocks,
			int32 Index, double RightEdgeY
		) const
		{
			bool bIsPast = false;

			int32 NextIndex = Index + 1;

			bool bHasNextBlock = this->Contains(NextIndex);

			if (bHasNextBlock)
			{
				const ABlock* NextBlock =
					Blocks[NextIndex];

				FVector NextLocation =
					NextBlock->GetActorLocation();

				double NextLocationY = NextLocation.Y;

				bIsPast = RightEdgeY > NextLocationY;
			}

			return bIsPast;
		}

		bool IsPastPreviousBlock(
			const TArray<TObjectPtr<ABlock>>& Blocks,
			int32 Index, double LeftEdgeY
		) const
		{
			bool bIsPast = false;

			int32 PreviousIndex = Index - 1;

			bool bHasPreviousBlock =
				this->Contains(PreviousIndex);

			if (bHasPreviousBlock)
			{
				const ABlock* PreviousBlock =
					Blocks[PreviousIndex];

				FVector PreviousLocation =
					PreviousBlock->GetActorLocation();

				bIsPast = LeftEdgeY < PreviousLocation.Y;
			}

			return bIsPast;
		}
	
	#pragma endregion
	
	private:

		int32 FirstIndex = 0;

		int32 LastIndex = 0;

		double Width = 0.0;

		FVector Start = FVector::ZeroVector;

		double MaxWidth = 0.0;
};