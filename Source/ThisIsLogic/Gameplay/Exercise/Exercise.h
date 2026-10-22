#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Components/SceneComponent.h"

#include "UndoHistory.h"
#include "../Formula/Formula.h"
#include "../Block/BlockData.h"
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
		}

		void SetFormula(AFormula* NewFormula)
		{
			this->Formula = NewFormula;
		}

		void SetStartingFormula(const FString& Text)
		{
			this->StartingTokens = this->AnalyzeFormula(Text);
		}

		void Restart()
		{
			this->Formula->SetTokens(this->StartingTokens);

			this->SaveState();
		}

		void Undo()
		{
			bool bCanUndo = this->UndoHistory.CanUndo();

			if (bCanUndo)
			{
				TArray<FBlockState> Snapshot =
					this->UndoHistory.Undo();

				this->Formula->SetStates(Snapshot);
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

		void SaveState()
		{
			TArray<FBlockState> Snapshot =
				this->Formula->GetStates();

			this->UndoHistory.Save(Snapshot);
		}
	
		void OnFormulaChanged()
		{
			this->SaveState();
		}

	#pragma endregion

	#pragma region Attributes
	
		UPROPERTY()
		TObjectPtr<AFormula> Formula;

		TArray<FToken> StartingTokens;
	
		FUndoHistory UndoHistory;
	
	#pragma endregion
};