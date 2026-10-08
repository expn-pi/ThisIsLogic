#pragma once

#include "CoreMinimal.h"

#include "Block.h"

#include "OperatorBlock.generated.h"

UCLASS()
class THISISLOGIC_API AOperatorBlock :
	public ABlock
{
	GENERATED_BODY()

	public:

		AOperatorBlock()
		{
			FLinearColor BlockColor =
				FLinearColor(0.1f, 0.3f, 0.8f);

			this->SetColor(BlockColor);
		}

		void SetSymbol(const FString& NewSymbol)
		{
			this->Symbol = NewSymbol;
		}

	protected:

		virtual FString GetText() const override
		{
			return this->Symbol;
		}

	private:

		FString Symbol;
};