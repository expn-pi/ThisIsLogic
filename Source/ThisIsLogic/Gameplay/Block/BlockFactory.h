#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Templates/SubclassOf.h"

#include "Block.h"
#include "PropositionBlock.h"
#include "OperatorBlock.h"
#include "../../Logic/LogicTypes.h"

class FBlockFactory
{
	public:

		FBlockFactory() = delete;

		static ABlock* CreateBlock(
			UWorld* World, const FToken& Token
		)
		{
			FTransform Transform =
				FTransform::Identity;

			TSubclassOf<ABlock> BlockClass =
				FBlockFactory::GetBlockClass(Token);

			ABlock* Block =
				World->
				SpawnActorDeferred<
					ABlock
				>(BlockClass, Transform);

			FString TokenText = Token.Text;

			Block->SetText(TokenText);

			Block->FinishSpawning(Transform);

			return Block;
		}

	private:

		static TSubclassOf<ABlock> GetBlockClass(
			const FToken& Token
		)
		{
			bool bIsProposition =
				Token.Kind == ETokenKind::Proposition;

			if (bIsProposition)
			{
				return APropositionBlock::StaticClass();
			}

			return AOperatorBlock::StaticClass();
		}
};