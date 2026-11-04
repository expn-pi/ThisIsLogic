#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TextRenderComponent.h"

#include "Indicator.h"
#include "../Notation/Notation.h"
#include "../../Visuals/RoundedBackgroundComponent.h"

#include "InfoPanel.generated.h"

UCLASS()
class THISISLOGIC_API UInfoPanel : public USceneComponent
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		UInfoPanel()
		{
			FName StatementName = TEXT("StatementBackground");

			this->StatementBackground =
				this->CreateBackground(StatementName);

			FName IndicatorsName = TEXT("IndicatorsBackground");

			this->IndicatorsBackground =
				this->CreateBackground(IndicatorsName);

			this->CreateStatementLabel();

			FName ValidityName = TEXT("Validity");

			this->Validity = this->CreateIndicator(ValidityName);

			FName EquivalenceName = TEXT("Equivalence");

			this->Equivalence =
				this->CreateIndicator(EquivalenceName);
		}

		void SetData(
			const FString& NewStatementText,
			UMaterialInterface* BaseMaterial, UFont* Font,
			UMaterialInterface* TextMaterial
		)
		{
			this->StatementBackground->
				SetBaseMaterial(BaseMaterial);

			this->IndicatorsBackground->
				SetBaseMaterial(BaseMaterial);

			this->StatementText = NewStatementText;

			this->StatementLabel->SetFont(Font);

			this->StatementLabel->SetTextMaterial(TextMaterial);

			FString ValidityText = "Well-formed";

			this->Validity->
				SetData(
					ValidityText, BaseMaterial,
					Font, TextMaterial
				);

			FString EquivalenceText = "Equivalent";

			this->Equivalence->
				SetData(
					EquivalenceText, BaseMaterial,
					Font, TextMaterial
				);
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

			float Height = UInfoPanel::GetPanelHeight();

			float IndicatorsWidth =
				this->BuildIndicators(Height);

			this->BuildStatement(IndicatorsWidth, Height);
		}

	#pragma region PrivateMethods
	private:

		// Creation

		URoundedBackgroundComponent* CreateBackground(
			FName BackgroundName
		)
		{
			URoundedBackgroundComponent* Background =
				CreateDefaultSubobject<
					URoundedBackgroundComponent
				>(BackgroundName);

			Background->SetupAttachment(this);

			Background->
				SetCollisionEnabled(
					ECollisionEnabled::NoCollision
				);

			FColor ColorSRGB = FColor(0xFF2A3039);

			FLinearColor Color = FLinearColor(ColorSRGB);

			Background->SetColor(Color);

			float CornerRadius =
				UInfoPanel::CircleDiameter * 0.4f;

			Background->SetCornerRadius(CornerRadius);

			return Background;
		}

		void CreateStatementLabel()
		{
			FName LabelName = TEXT("StatementLabel");

			this->StatementLabel =
				CreateDefaultSubobject<
					UTextRenderComponent
				>(LabelName);

			this->StatementLabel->SetupAttachment(this);

			FRotator Rotation =
				FRotator(90.f, 180.f, 0.f);

			this->StatementLabel->
				SetRelativeRotation(Rotation);

			this->StatementLabel->
				SetHorizontalAlignment(EHTA_Center);

			this->StatementLabel->
				SetVerticalAlignment(EVRTA_TextCenter);

			this->StatementLabel->TextRenderColor =
				FColor::White;

			float Size = UInfoPanel::CircleDiameter * 0.6f;

			this->StatementLabel->SetWorldSize(Size);
		}

		UIndicator* CreateIndicator(FName IndicatorName)
		{
			UIndicator* Indicator =
				CreateDefaultSubobject<
					UIndicator
				>(IndicatorName);

			Indicator->SetupAttachment(this);

			return Indicator;
		}

		// Building

		static float GetPanelHeight()
		{
			float Diameter = UInfoPanel::CircleDiameter;

			float Margin = UInfoPanel::BackgroundMargin;

			float RowGap = Diameter * 0.1f;

			float Height =
				Diameter * 2.f + RowGap + Margin * 2.f;

			return Height;
		}

		float BuildIndicators(float Height)
		{
			float Diameter = UInfoPanel::CircleDiameter;

			float ValidityWidth = this->Validity->Build(Diameter);

			float EquivalenceWidth =
				this->Equivalence->Build(Diameter);

			float ContentWidth =
				FMath::Max(ValidityWidth, EquivalenceWidth);

			float Margin = UInfoPanel::BackgroundMargin;

			float Width = ContentWidth + Margin * 2.f;

			float CenterY = -Width * 0.5f;

			UInfoPanel::
				BuildBackground(
					this->IndicatorsBackground,
					Width, Height, CenterY
				);

			float LeftY = Margin - Width;

			float RowX = (Height - Diameter) * 0.5f - Margin;

			FVector ValidityLocation =
				FVector(RowX, LeftY, 0.f);

			this->Validity->
				SetRelativeLocation(ValidityLocation);

			FVector EquivalenceLocation =
				FVector(-RowX, LeftY, 0.f);

			this->Equivalence->
				SetRelativeLocation(EquivalenceLocation);

			return Width;
		}

		void BuildStatement(
			float IndicatorsWidth, float Height
		)
		{
			float Margin = UInfoPanel::BackgroundMargin;

			float Gap = UInfoPanel::CircleDiameter * 0.15f;

			float RightY = -IndicatorsWidth - Gap;

			float Width =
				UInfoPanel::PanelWidth - IndicatorsWidth - Gap;

			float CenterY = RightY - Width * 0.5f;

			UInfoPanel::
				BuildBackground(
					this->StatementBackground,
					Width, Height, CenterY
				);

			FVector LabelLocation =
				FVector(0.f, CenterY, 0.f);

			this->StatementLabel->
				SetRelativeLocation(LabelLocation);

			float MaxWidth = Width - Margin * 2.f;

			float MaxHeight = Height - Margin * 2.f;

			this->FitStatement(MaxWidth, MaxHeight);
		}
	
		void FitStatement(float MaxWidth, float MaxHeight)
		{
			FString Wrapped = this->WrapStatement(MaxWidth);

			FText WrappedText = FText::FromString(Wrapped);

			this->StatementLabel->SetText(WrappedText);

			FVector TextSize =
				this->StatementLabel->GetTextLocalSize();

			double TextSizeZ = TextSize.Z;

			float TextHeight = static_cast<float>(TextSizeZ);

			bool bFitsHeight = TextHeight <= MaxHeight;

			check(bFitsHeight);
		}

		FString WrapStatement(float MaxWidth)
		{
			FString Space = " ";

			FString LineBreak = "<br>";

			TArray<FString> Words;

			this->StatementText.ParseIntoArray(Words, *Space);

			FString Wrapped;

			FString Line;

			for (const FString& Word : Words)
			{
				FString Candidate;

				if (Line.IsEmpty())
				{
					Candidate = Word;
				}
				else
				{
					Candidate = Line + Space + Word;
				}

				float CandidateWidth =
					this->MeasureStatementWidth(Candidate);

				bool bFits = CandidateWidth <= MaxWidth;

				if (bFits || Line.IsEmpty())
				{
					Line = Candidate;
				}
				else
				{
					Wrapped += Line + LineBreak;

					Line = Word;
				}
			}

			Wrapped += Line;

			return Wrapped;
		}

		float MeasureStatementWidth(const FString& Text)
		{
			FText LabelText = FText::FromString(Text);

			this->StatementLabel->SetText(LabelText);

			FVector TextSize =
				this->StatementLabel->GetTextLocalSize();

			double TextSizeY = TextSize.Y;

			float TextWidth = static_cast<float>(TextSizeY);

			return TextWidth;
		}

		static void BuildBackground(
			URoundedBackgroundComponent* Background,
			float Width, float Height, float CenterY
		)
		{
			Background->Build(Width, Height);

			float Depth = UInfoPanel::CircleDiameter * 0.1f;

			FVector Location =
				FVector(0.f, CenterY, -Depth);

			Background->SetRelativeLocation(Location);
		}

		// Showing

		void ShowValidity(bool bIsWellFormed)
		{
			FLinearColor Color =
				UInfoPanel::GetAnswerColor(bIsWellFormed);

			FString Symbol =
				UInfoPanel::GetValiditySymbol(bIsWellFormed);

			this->Validity->Show(Color, Symbol);
		}

		void ShowEquivalence(
			bool bIsWellFormed, bool bIsEquivalent
		)
		{
			FLinearColor Color =
				UInfoPanel::
					GetEquivalenceColor(
						bIsWellFormed, bIsEquivalent
					);

			FString Symbol =
				this->GetEquivalenceSymbol(
					bIsWellFormed, bIsEquivalent
				);

			this->Equivalence->Show(Color, Symbol);
		}

		// Symbols

		static FString GetValiditySymbol(bool bIsWellFormed)
		{
			TCHAR CheckCode = 0x2714;

			TCHAR CrossCode = 0x2717;

			if (bIsWellFormed)
			{
				return FString::Chr(CheckCode);
			}
			else
			{
				return FString::Chr(CrossCode);
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
				TCHAR NoAnswerCode = 0x2205;

				return FString::Chr(NoAnswerCode);
			}
		}

		// Colors

		static FLinearColor GetEquivalenceColor(
			bool bIsWellFormed, bool bIsEquivalent
		)
		{
			if (bIsWellFormed)
			{
				return UInfoPanel::GetAnswerColor(bIsEquivalent);
			}
			else
			{
				return UInfoPanel::GetNoAnswerColor();
			}
		}

		static FLinearColor GetAnswerColor(bool bAnswer)
		{
			FColor SRGBColor;

			if (bAnswer)
			{
				SRGBColor = FColor(0xFF16D7D7);
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
			FColor SRGBColor = FColor(0xFFBB86F2);

			FLinearColor Color = FLinearColor(SRGBColor);

			return Color;
		}

	#pragma endregion

	#pragma region Attributes

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent>
			StatementBackground;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<URoundedBackgroundComponent>
			IndicatorsBackground;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UTextRenderComponent> StatementLabel;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UIndicator> Validity;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UIndicator> Equivalence;

		FNotation Notation;
	
		FString StatementText;

		static constexpr float CircleDiameter = 2.f;

		static constexpr float BackgroundMargin =
			UInfoPanel::CircleDiameter * 0.3f;

		static constexpr float PanelWidth = 41.2f;

	#pragma endregion
};