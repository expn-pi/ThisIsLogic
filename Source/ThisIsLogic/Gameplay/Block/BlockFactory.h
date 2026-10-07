#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Templates/SubclassOf.h"

#include "Block.h"
#include "../../Logic/LogicTypes.h"

class FBlockFactory
{
	public:

		FBlockFactory(
			UWorld* InWorld,
			TSubclassOf<ABlock> InBlockClass
		)
		{
			this->World = InWorld;
			this->BlockClass = InBlockClass;
		}

		ABlock* CreateBlock(
			const FToken& Token
		) const
		{
			FTransform Transform =
				FTransform::Identity;

			ABlock* Block =
				this->World->
					SpawnActorDeferred<
						ABlock
						>(
							this->BlockClass,
							Transform
						);

			FString TokenText = Token.Text;

			Block->SetText(TokenText);

			Block->FinishSpawning(Transform);

			return Block;
		}

	private:

		UWorld* World;

		TSubclassOf<ABlock> BlockClass;
};
