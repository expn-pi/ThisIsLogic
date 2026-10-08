#pragma once

#include "CoreMinimal.h"

#include "Block.h"

#include "PropositionBlock.generated.h"

UCLASS()
class THISISLOGIC_API APropositionBlock :
	public ABlock
{
	GENERATED_BODY()

	public:

		APropositionBlock()
		{
			FLinearColor BlockColor =
				FLinearColor(0.9f, 0.3f, 0.1f);

			this->SetColor(BlockColor);
		}
};