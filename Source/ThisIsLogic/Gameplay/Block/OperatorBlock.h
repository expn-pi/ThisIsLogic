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

		virtual void OnConstruction(const FTransform& Transform)
			override
		{
			FLinearColor BlockColor = this->GetBlockColor();

			this->SetColor(BlockColor);

			Super::OnConstruction(Transform);
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

		FLinearColor GetBlockColor() const
		{
			FToken BlockToken = this->GetToken();

			ETokenKind Kind = BlockToken.Kind;

			bool bIsParenthesis =
				Kind == ETokenKind::OpenParenthesis ||
				Kind == ETokenKind::CloseParenthesis;

			FColor SRGBColor;

			if (bIsParenthesis)
			{
				SRGBColor = FColor(0xFFB4BBC4);
			}
			else
			{
				SRGBColor = FColor(0xFF5EB8EA);
			}

			FLinearColor BlockColor = FLinearColor(SRGBColor);

			return BlockColor;
		}

		FString Symbol;
};