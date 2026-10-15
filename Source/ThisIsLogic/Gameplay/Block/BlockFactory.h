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

		FBlockFactory(
			TCHAR InFirstLetter,
			ESymbolStyle SymbolStyle,
			float InBlockHeight
		) :
			Notation(InFirstLetter, SymbolStyle),
			BlockHeight(InBlockHeight)
		{
		}

		TArray<ABlock*> CreateBlocks(
			UWorld* World,
			const TArray<FToken>& Tokens,
			const TArray<FString>& Sentences
		) const
		{
			TArray<ABlock*> Blocks;

			for (const FToken& Token : Tokens)
			{
				ABlock* Block =
					this->CreateBlock(
						World, Token, Sentences
					);

				Blocks.Add(Block);
			}

			return Blocks;
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
				return
					this->CreatePropositionBlock(
						World, Token, Sentences
					);
			}
			else
			{
				return
					this->CreateOperatorBlock(
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

			Block->SetHeight(this->BlockHeight);
			
			int32 PropositionIndex =
				Token.Index;

			FString Letter =
				this->Notation.GetLetter(PropositionIndex);

			const FString& Sentence =
				Sentences[PropositionIndex];

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
			
			Block->SetHeight(this->BlockHeight);

			ETokenKind Kind = Token.Kind;

			FString Symbol =
				this->Notation.GetSymbol(Kind);

			Block->SetSymbol(Symbol);

			Block->FinishSpawning(Transform);

			return Block;
		}

		FNotation Notation;
	
		float BlockHeight = 0.f;
};