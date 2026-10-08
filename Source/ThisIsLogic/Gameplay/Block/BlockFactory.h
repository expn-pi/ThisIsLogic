#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

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
			bool bIsProposition =
				Token.Kind == ETokenKind::Proposition;

			if (bIsProposition)
			{
				return FBlockFactory::
					CreatePropositionBlock(World, Token);
			}
			else
			{
				return FBlockFactory::
					CreateOperatorBlock(World, Token);
			}
		}

	private:

		static APropositionBlock* CreatePropositionBlock(
			UWorld* World, const FToken& Token
		)
		{
			FTransform Transform =
				FTransform::Identity;

			UClass* BlockClass =
				APropositionBlock::StaticClass();

			APropositionBlock* Block =
				World->
					SpawnActorDeferred<
						APropositionBlock
					>(BlockClass, Transform);

			const FString& Sentence = Token.Text;

			Block->SetSentence(Sentence);

			Block->FinishSpawning(Transform);

			return Block;
		}

		static AOperatorBlock* CreateOperatorBlock(
			UWorld* World, const FToken& Token
		)
		{
			FTransform Transform =
				FTransform::Identity;

			UClass* BlockClass =
				AOperatorBlock::StaticClass();

			AOperatorBlock* Block =
				World->
					SpawnActorDeferred<
						AOperatorBlock
					>(BlockClass, Transform);

			const FString& Symbol = Token.Text;

			Block->SetSymbol(Symbol);

			Block->FinishSpawning(Transform);

			return Block;
		}
};