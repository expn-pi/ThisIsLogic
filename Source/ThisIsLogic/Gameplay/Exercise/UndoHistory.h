#pragma once

#include "CoreMinimal.h"

#include "../Block/BlockData.h"

class FUndoHistory
{
	public:

		void Save(const TArray<FBlockState>& Snapshot)
		{
			bool bIsEqual = this->IsCurrent(Snapshot);
			
			bool bIsDifferent = ! bIsEqual;

			if (bIsDifferent)
			{
				this->Snapshots.Push(Snapshot);
			}
		}

		bool CanUndo() const
		{
			int32 SnapshotCount = this->Snapshots.Num();

			return SnapshotCount > 1;
		}

		TArray<FBlockState> Undo()
		{
			int32 SnapshotCount = this->Snapshots.Num();

			int32 CurrentIndex = SnapshotCount - 1;

			this->Snapshots.RemoveAt(CurrentIndex);

			return this->Snapshots.Last();
		}
	
		void Clear()
		{
			this->Snapshots.Empty();
		}

	private:

		bool IsCurrent(const TArray<FBlockState>& Snapshot)
			const
		{
			bool bIsCurrent = false;

			bool bHasSnapshot = !this->Snapshots.IsEmpty();

			if (bHasSnapshot)
			{
				const TArray<FBlockState>& Current =
					this->Snapshots.Last();

				bIsCurrent = Snapshot == Current;
			}

			return bIsCurrent;
		}

		TArray<TArray<FBlockState>> Snapshots;
};