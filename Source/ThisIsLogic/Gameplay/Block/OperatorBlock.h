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
			FColor FBlockColor = FColor(0xFF5EB8EA);
			
			FLinearColor BlockColor = FLinearColor(FBlockColor);

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
	
	virtual float GetLabelSize() const override
		{
			return
				this->Height *
				AOperatorBlock::LabelSizeRatio;
		}
	
		static constexpr float LabelSizeRatio = 0.7f;
	
	private:

		FString Symbol;
};