#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"

#include "Indicators.h"
#include "UndoHistory.h"
#include "../Formula/Formula.h"
#include "../Block/BlockData.h"
#include "../../Logic/FormulaParser.h"
#include "../../Logic/FormulaAnalyzer.h"

#include "Exercise.generated.h"

UCLASS()
class THISISLOGIC_API AExercise : public AActor
{
	GENERATED_BODY()

	#pragma region PublicMethods
	public:

		AExercise()
		{
			this->PrimaryActorTick.bCanEverTick = false;

			FName RootName = TEXT("Root");

			USceneComponent* Root =
				CreateDefaultSubobject<USceneComponent>(RootName);

			this->RootComponent = Root;

			FName IndicatorsName = TEXT("Indicators");

			this->Indicators =
				CreateDefaultSubobject<
					UIndicators
				>(IndicatorsName);

			this->Indicators->SetupAttachment(Root);
		}

		void SetFormula(AFormula* NewFormula)
		{
			this->Formula = NewFormula;
		}

		void SetIndicatorsAssets(
			UMaterialInterface* Material, UFont* Font,
			UMaterialInterface* TextMaterial
		)
		{
			this->Indicators->
				SetAssets(Material, Font, TextMaterial);
		}

		void SetFormulasTexts(
			const FString& StartingText,
			const FString& SolutionText
		)
		{
			this->StartingTokens =
				this->AnalyzeFormula(StartingText);
		
			TArray<FToken> SolutionTokens =
				this->AnalyzeFormula(SolutionText);

			FExpression Starting =
				AExercise::ParseLevelFormula(this->StartingTokens);

			FExpression Solution =
				AExercise::ParseLevelFormula(SolutionTokens);

			bool bIsEquivalent = Solution.IsEquivalentTo(Starting);

			check(bIsEquivalent);

			this->SolutionExpression = Solution;
		}

		void Restart()
		{
			this->Formula->SetTokens(this->StartingTokens);

			this->SaveState();

			this->UpdateIndicators();
		}

		void Undo()
		{
			bool bCanUndo = this->UndoHistory.CanUndo();

			if (bCanUndo)
			{
				TArray<FBlockState> Snapshot =
					this->UndoHistory.Undo();

				this->Formula->SetStates(Snapshot);

				this->UpdateIndicators();
			}
		}
	
	#pragma endregion

	protected:

		virtual void BeginPlay() override
		{
			Super::BeginPlay();

			this->ConnectFormula();

			this->Formula->SetTokens(this->StartingTokens);

			this->SaveState();

			this->UpdateIndicators();
		}

	#pragma region PrivateMethods
	private:

		void ConnectFormula()
		{
			bool bHasFormula = this->Formula != nullptr;

			check(bHasFormula);

			this->Formula->
				SetChangedListener(
					this, &AExercise::OnFormulaChanged
				);
		}

		TArray<FToken> AnalyzeFormula(const FString& Text)
			const
		{
			const TCHAR* FormulaChars = *Text;

			std::string FormulaText = TCHAR_TO_UTF8(FormulaChars);

			std::optional<std::vector<FToken>>
				AnalyzedTokens =
					FFormulaAnalyzer::Analyze(FormulaText);

			bool bIsValid = AnalyzedTokens.has_value();

			check(bIsValid);

			const std::vector<FToken>& ValidTokens =
				AnalyzedTokens.value();

			const FToken* TokenData = ValidTokens.data();

			std::size_t TokenCount = ValidTokens.size();

			int32 TokenNum = static_cast<int32>(TokenCount);

			TArray<FToken> Tokens =
				TArray<FToken>(TokenData, TokenNum);

			return Tokens;
		}

		static std::optional<FExpression> ParseTokens(
			const TArray<FToken>& Tokens
		)
		{
			std::vector<FToken> StandardTokens =
				AExercise::ToStandardTokens(Tokens);

			return FFormulaParser::Parse(StandardTokens);
		}

		static FExpression ParseLevelFormula(
			const TArray<FToken>& Tokens
		)
		{
			std::optional<FExpression> Expression =
				AExercise::ParseTokens(Tokens);

			bool bIsWellFormed = Expression.has_value();

			check(bIsWellFormed);

			return Expression.value();
		}
	
		static std::vector<FToken> ToStandardTokens(
			const TArray<FToken>& Tokens
		)
		{
			const FToken* TokenData = Tokens.GetData();

			int32 TokenNum = Tokens.Num();

			const FToken* TokenEnd = TokenData + TokenNum;

			std::vector<FToken> StandardTokens =
				std::vector<FToken>(TokenData, TokenEnd);

			return StandardTokens;
		}
	
		void SaveState()
		{
			TArray<FBlockState> Snapshot =
				this->Formula->GetStates();

			this->UndoHistory.Save(Snapshot);
		}
	
		void OnFormulaChanged()
		{
			this->SaveState();

			this->UpdateIndicators();
		}

		void UpdateIndicators()
		{
			TArray<FToken> Tokens = this->Formula->GetTokens();

			std::optional<FExpression> Expression =
				AExercise::ParseTokens(Tokens);

			bool bIsWellFormed = Expression.has_value();

			bool bIsEquivalent = false;

			if (bIsWellFormed)
			{
				const FExpression& ValidExpression =
					Expression.value();

				const FExpression& Solution =
					this->SolutionExpression.value();

				bIsEquivalent =
					ValidExpression.IsEquivalentTo(Solution);
			}

			this->Indicators->Show(bIsWellFormed, bIsEquivalent);
		}

	#pragma endregion

	#pragma region Attributes

		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		UPROPERTY(VisibleAnywhere)
		TObjectPtr<UIndicators> Indicators;

		TArray<FToken> StartingTokens;

		std::optional<FExpression> SolutionExpression;

		FUndoHistory UndoHistory;

	#pragma endregion
};