#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"

#include "../Notation/Notation.h"
#include "../../Visuals/RoundedBackgroundComponent.h"

#include "Indicators.generated.h"

UCLASS()
class THISISLOGIC_API UIndicators : public USceneComponent
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		UIndicators()
		{
			this->CreateBackground();

			this->CreateCircles();

			this->CreateSymbols();

			this->CreateLabels();
		}

		void SetAssets(
			UMaterialInterface* BaseMaterial, UFont* Font,
			UMaterialInterface* TextMaterial
		)
		{
			this->Background->SetBaseMaterial(BaseMaterial);

			this->ValidityCircle->SetBaseMaterial(BaseMaterial);

			this->EquivalenceCircle->SetBaseMaterial(BaseMaterial);

			this->ValiditySymbol->SetFont(Font);

			this->ValiditySymbol->SetTextMaterial(TextMaterial);

			this->EquivalenceSymbol->SetFont(Font);

			this->EquivalenceSymbol->SetTextMaterial(TextMaterial);

			this->ValidityLabel->SetFont(Font);

			this->ValidityLabel->SetTextMaterial(TextMaterial);

			this->EquivalenceLabel->SetFont(Font);

			this->EquivalenceLabel->SetTextMaterial(TextMaterial);
		}

		void Show(bool bIsWellFormed, bool bIsEquivalent)
		{
			this->ShowValidity(bIsWellFormed);

			this->ShowEquivalence(bIsWellFormed, bIsEquivalent);
		}

	#pragma endregion

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			float Size = UIndicators::CircleDiameter;

			this->ValidityCircle->
				Build(Size, Size);

			this->EquivalenceCircle->
				Build(Size, Size);

			float Width = this->LayoutIndicators();

			this->BuildBackground(Width);
		}

	#pragma region PrivateMethods
	private:

		// Creation

		void CreateBackground()
		{
			FName BackgroundName = TEXT("Background");

			this->Background = this->CreatePart(BackgroundName);

			FColor ColorSRGB = FColor(0xFF2A3039);

			FLinearColor Color = FLinearColor(ColorSRGB);

			this->Background->SetColor(Color);

			float CornerRadius = UIndicators::BackgroundRadius;

			this->Background->SetCornerRadius(CornerRadius);
		}

		void CreateCircles()
		{
			FName ValidityName = TEXT("ValidityCircle");

			this->ValidityCircle =
				this->CreateCircle(ValidityName);

			FName EquivalenceName = TEXT("EquivalenceCircle");

			this->EquivalenceCircle =
				this->CreateCircle(EquivalenceName);
		}

		void CreateSymbols()
		{
			FName ValidityName = TEXT("ValiditySymbol");

			this->ValiditySymbol =
				this->CreateSymbol(
					ValidityName, this->ValidityCircle
				);

			FName EquivalenceName = TEXT("EquivalenceSymbol");

			this->EquivalenceSymbol =
				this->CreateSymbol(
					EquivalenceName, this->EquivalenceCircle
				);
		}

		void CreateLabels()
		{
			FName ValidityName = TEXT("ValidityLabel");

			FString ValidityText = "Well-formed:";

			this->ValidityLabel =
				this->CreateLabel(
					ValidityName, ValidityText
				);

			FName EquivalenceName = TEXT("EquivalenceLabel");

			FString EquivalenceText = "Equivalent:";

			this->EquivalenceLabel =
				this->CreateLabel(
					EquivalenceName, EquivalenceText
				);
		}

		URoundedBackgroundComponent* CreateCircle(
			FName CircleName
		)
		{
			URoundedBackgroundComponent* Circle =
				this->CreatePart(CircleName);

			Circle->SetRoundness(1.f);

			return Circle;
		}

		URoundedBackgroundComponent* CreatePart(
			FName PartName
		)
		{
			URoundedBackgroundComponent* Part =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(PartName);

			Part->SetupAttachment(this);

			Part->
				SetCollisionEnabled(
					ECollisionEnabled::NoCollision
				);

			return Part;
		}

		UTextRenderComponent* CreateSymbol(
			FName SymbolName, URoundedBackgroundComponent* Circle
		)
		{
			UTextRenderComponent* Symbol =
				this->CreateTextComponent(SymbolName, Circle);

			Symbol->TextRenderColor = FColor::Black;

			Symbol->SetWorldSize(UIndicators::SymbolSize);

			float LayerOffset = UIndicators::SymbolLayerOffset;

			FVector Location =
				FVector(0.f, 0.f, LayerOffset);

			Symbol->SetRelativeLocation(Location);

			return Symbol;
		}

		UTextRenderComponent* CreateLabel(
			FName LabelName, const FString& Text
		)
		{
			UTextRenderComponent* Label =
				this->CreateTextComponent(LabelName, this);

			Label->TextRenderColor = FColor::White;

			Label->SetWorldSize(UIndicators::LabelSize);

			FText LabelText = FText::FromString(Text);

			Label->SetText(LabelText);

			return Label;
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

		// Layout

		float LayoutIndicators()
		{
			float EquivalenceRightY =
				-UIndicators::BackgroundMargin;

			float EquivalenceLeftY =
				UIndicators::
					PlaceIndicator(
						this->EquivalenceLabel,
						this->EquivalenceCircle,
						EquivalenceRightY
					);

			float ValidityRightY =
				EquivalenceLeftY - UIndicators::IndicatorGap;

			float ValidityLeftY =
				UIndicators::
					PlaceIndicator(
						this->ValidityLabel,
						this->ValidityCircle,
						ValidityRightY
					);

			float Width =
				UIndicators::BackgroundMargin - ValidityLeftY;

			return Width;
		}

		static float PlaceIndicator(
			UTextRenderComponent* Label,
			URoundedBackgroundComponent* Circle,
			float RightY
		)
		{
			float Size = UIndicators::CircleDiameter;

			float CircleY = RightY - Size * 0.5f;

			FVector CircleLocation =
				FVector(0.f, CircleY, 0.f);

			Circle->SetRelativeLocation(CircleLocation);

			float LabelRightY =
				RightY - Size - UIndicators::LabelGap;

			float LabelWidth = UIndicators::GetLabelWidth(Label);

			float LabelY = LabelRightY - LabelWidth * 0.5f;

			FVector LabelLocation =
				FVector(0.f, LabelY, 0.f);

			Label->SetRelativeLocation(LabelLocation);

			float LeftY = LabelRightY - LabelWidth;

			return LeftY;
		}

		static float GetLabelWidth(
			UTextRenderComponent* Label
		)
		{
			FVector TextSize = Label->GetTextLocalSize();

			double TextSizeY = TextSize.Y;

			float TextWidth = static_cast<float>(TextSizeY);

			return TextWidth;
		}

		void BuildBackground(float Width)
		{
			float Height = UIndicators::BackgroundHeight;

			this->Background->Build(Width, Height);

			float CenterY = -Width * 0.5f;

			float Depth = UIndicators::BackgroundDepth;

			FVector Location =
				FVector(0.f, CenterY, -Depth);

			this->Background->SetRelativeLocation(Location);
		}

		// Showing

		void ShowValidity(bool bIsWellFormed)
		{
			FLinearColor Color =
				UIndicators::GetAnswerColor(bIsWellFormed);

			this->ValidityCircle->ChangeColor(Color);

			FString Symbol =
				UIndicators::GetValiditySymbol(bIsWellFormed);

			FText SymbolText = FText::FromString(Symbol);

			this->ValiditySymbol->SetText(SymbolText);
		}

		void ShowEquivalence(
			bool bIsWellFormed, bool bIsEquivalent
		)
		{
			FLinearColor Color =
				UIndicators::
					GetEquivalenceColor(
						bIsWellFormed, bIsEquivalent
					);

			this->EquivalenceCircle->ChangeColor(Color);

			FString Symbol =
				this->GetEquivalenceSymbol(
					bIsWellFormed, bIsEquivalent
				);

			FText SymbolText = FText::FromString(Symbol);

			this->EquivalenceSymbol->SetText(SymbolText);
		}

		// Symbols

		static FString GetValiditySymbol(bool bIsWellFormed)
		{
			if (bIsWellFormed)
			{
				return FString::Chr(UIndicators::CheckCode);
			}
			else
			{
				return FString::Chr(UIndicators::CrossCode);
			}
		}

		FString GetEquivalenceSymbol(
			bool bIsWellFormed, bool bIsEquivalent
		) const
		{
			if (bIsWellFormed)
			{
				return
					this->Notation.
						GetEquivalentSymbol(bIsEquivalent);
			}
			else
			{
				return FString::Chr(UIndicators::NoAnswerCode);
			}
		}

		// Colors

		static FLinearColor GetEquivalenceColor(
			bool bIsWellFormed, bool bIsEquivalent
		)
		{
			if (bIsWellFormed)
			{
				return UIndicators::GetAnswerColor(bIsEquivalent);
			}
			else
			{
				return UIndicators::GetNoAnswerColor();
			}
		}

		static FLinearColor GetAnswerColor(bool bAnswer)
		{
			FColor SRGBColor;

			if (bAnswer)
			{
				SRGBColor = FColor(0xFF22D3EE);
			}
			else
			{
				SRGBColor = FColor(0xFFEF4444);
			}

			FLinearColor Color = FLinearColor(SRGBColor);

			return Color;
		}

		static FLinearColor GetNoAnswerColor()
		{
			FColor SRGBColor = FColor(0xFF8B949E);

			FLinearColor Color = FLinearColor(SRGBColor);

			return Color;
		}

	#pragma endregion

	#pragma region Attributes

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> Background;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> ValidityCircle;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent> EquivalenceCircle;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> ValiditySymbol;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> EquivalenceSymbol;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> ValidityLabel;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> EquivalenceLabel;

		FNotation Notation;

		static constexpr float CircleDiameter = 2.f;

		static constexpr float SymbolSize =
			UIndicators::CircleDiameter * 0.7f;

		static constexpr float SymbolLayerOffset =
			UIndicators::CircleDiameter * 0.05f;

		static constexpr float LabelSize =
			UIndicators::CircleDiameter * 0.6f;

		static constexpr float LabelGap =
			UIndicators::CircleDiameter * 0.3f;

		static constexpr float IndicatorGap =
			UIndicators::CircleDiameter * 0.6f;

		static constexpr float BackgroundMargin =
			UIndicators::CircleDiameter * 0.3f;

		static constexpr float BackgroundHeight =
			UIndicators::CircleDiameter +
			UIndicators::BackgroundMargin * 2.f;

		static constexpr float BackgroundRadius =
			UIndicators::CircleDiameter * 0.4f;

		static constexpr float BackgroundDepth =
			UIndicators::CircleDiameter * 0.1f;

		static constexpr TCHAR CheckCode = 0x2714;

		static constexpr TCHAR CrossCode = 0x2717;
	
		static constexpr TCHAR NoAnswerCode = 0x2205;

	#pragma endregion
};