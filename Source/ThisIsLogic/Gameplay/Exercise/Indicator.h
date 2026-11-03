#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"

#include "../../Visuals/RoundedBackgroundComponent.h"

#include "Indicator.generated.h"

UCLASS()
class THISISLOGIC_API UIndicator : public USceneComponent
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		UIndicator()
		{
			this->CreateCircle();

			this->CreateSymbol();

			this->CreateLabel();
		}

		void SetData(
			const FString& Text,
			UMaterialInterface* BaseMaterial, UFont* Font,
			UMaterialInterface* TextMaterial
		)
		{
			this->Circle->SetBaseMaterial(BaseMaterial);

			this->Symbol->SetFont(Font);

			this->Symbol->SetTextMaterial(TextMaterial);

			FText LabelText = FText::FromString(Text);

			this->Label->SetText(LabelText);

			this->Label->SetFont(Font);

			this->Label->SetTextMaterial(TextMaterial);
		}

		float Build(float Diameter)
		{
			this->Circle->Build(Diameter, Diameter);

			this->BuildSymbol(Diameter);

			float LabelSize = Diameter * 0.6f;

			this->Label->SetWorldSize(LabelSize);

			float Width = this->LayoutParts(Diameter);

			return Width;
		}

		void Show(
			const FLinearColor& NewColor,
			const FString& NewSymbol
		)
		{
			this->Circle->ChangeColor(NewColor);

			FText SymbolText = FText::FromString(NewSymbol);

			this->Symbol->SetText(SymbolText);
		}

	#pragma endregion

	#pragma region PrivateMethods
	private:

		// Creation

		void CreateCircle()
		{
			FName CircleName = TEXT("Circle");

			this->Circle =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(CircleName);

			this->Circle->SetupAttachment(this);

			this->Circle->
				SetCollisionEnabled(
					ECollisionEnabled::NoCollision
				);

			this->Circle->SetRoundness(1.f);
		}

		void CreateSymbol()
		{
			FName SymbolName = TEXT("Symbol");

			this->Symbol =
				this->CreateTextComponent(
					SymbolName, this->Circle
				);

			this->Symbol->TextRenderColor = FColor::Black;
		}

		void CreateLabel()
		{
			FName LabelName = TEXT("Label");

			this->Label =
				this->CreateTextComponent(LabelName, this);

			this->Label->TextRenderColor = FColor::White;
		}

		UTextRenderComponent* CreateTextComponent(
			FName ComponentName, USceneComponent* Parent
		)
		{
			UTextRenderComponent* TextComponent =
				CreateDefaultSubobject<
					UTextRenderComponent
				>(ComponentName);

			TextComponent->SetupAttachment(Parent);

			FRotator Rotation =
				FRotator(90.f, 180.f, 0.f);

			TextComponent->SetRelativeRotation(Rotation);

			TextComponent->
				SetHorizontalAlignment(EHTA_Center);

			TextComponent->
				SetVerticalAlignment(EVRTA_TextCenter);

			return TextComponent;
		}

		// Building

		void BuildSymbol(float Diameter)
		{
			float SymbolSize = Diameter * 0.7f;

			this->Symbol->SetWorldSize(SymbolSize);

			float LayerOffset = Diameter * 0.05f;

			FVector Location =
				FVector(0.f, 0.f, LayerOffset);

			this->Symbol->SetRelativeLocation(Location);
		}

		float LayoutParts(float Diameter)
		{
			float CircleY = Diameter * 0.5f;

			FVector CircleLocation =
				FVector(0.f, CircleY, 0.f);

			this->Circle->SetRelativeLocation(CircleLocation);

			float LabelGap = Diameter * 0.3f;

			float LabelLeftY = Diameter + LabelGap;

			float LabelWidth = this->GetLabelWidth();

			float LabelY = LabelLeftY + LabelWidth * 0.5f;

			FVector LabelLocation =
				FVector(0.f, LabelY, 0.f);

			this->Label->SetRelativeLocation(LabelLocation);

			float Width = LabelLeftY + LabelWidth;

			return Width;
		}

		float GetLabelWidth() const
		{
			FVector TextSize = this->Label->GetTextLocalSize();

			double TextSizeY = TextSize.Y;

			float TextWidth = static_cast<float>(TextSizeY);

			return TextWidth;
		}

	#pragma endregion

	#pragma region Attributes

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Circle;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> Symbol;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> Label;

	#pragma endregion
};