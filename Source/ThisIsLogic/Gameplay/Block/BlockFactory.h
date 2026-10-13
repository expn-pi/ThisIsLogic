#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"

#include "Block.h"
#include "OperatorBlock.h"
#include "PropositionBlock.h"
#include "../Notation/Notation.h"
#include "../../Logic/LogicTypes.h"

class FBlockFactory
{
	public:

		explicit FBlockFactory(TCHAR InFirstLetter) :
			Notation(InFirstLetter)
		{
		}

		ABlock* CreateBlock(
			UWorld* World,
			const FToken& Token,
			const TArray<FString>& Sentences
		) const
		{
			bool bIsProposition =
				Token.Kind == ETokenKind::Proposition;

			if (bIsProposition)
			{
				return this->CreatePropositionBlock(
					World, Token, Sentences
				);
			}
			else
			{
				return this->CreateOperatorBlock(
					World, Token
				);
			}
		}

	private:

		APropositionBlock* CreatePropositionBlock(
			UWorld* World,
			const FToken& Token,
			const TArray<FString>& Sentences
		) const
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

			int32 PropositionNumber =
				Token.PropositionNumber;

			FString Letter =
				this->Notation.GetLetter(PropositionNumber);

			int32 SentenceIndex = PropositionNumber - 1;

			const FString& Sentence =
				Sentences[SentenceIndex];

			Block->SetProposition(Letter, Sentence);

			Block->FinishSpawning(Transform);

			return Block;
		}

		AOperatorBlock* CreateOperatorBlock(
			UWorld* World, const FToken& Token
		) const
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

			ETokenKind Kind = Token.Kind;

			FString Symbol =
				this->Notation.GetSymbol(Kind);

			Block->SetSymbol(Symbol);

			Block->FinishSpawning(Transform);

			return Block;
		}

		FNotation Notation;
};