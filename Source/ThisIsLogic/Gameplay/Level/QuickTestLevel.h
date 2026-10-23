#pragma once

#include "CoreMinimal.h"

class FQuickTestLevel
{
	public:

		FQuickTestLevel() = delete;

		static FString GetFormula()
		{
			FString Formula =
				"1 = ( 2 AND 3 AND 4 )"
				" OR ( 2 AND 3 AND NOT 4 )";
				//" OR ( 2 AND NOT 3 )";

			return Formula;
		}

		static TArray<FString> GetSentences()
		{
			TArray<FString> Sentences = {
				"Alarm on",
				"Motion sensor on",
				"Window sensor on",
				"Door sensor on"
			};

			return Sentences;
		}
	
		static FString GetSolution()
		{
			FString Solution =
				"1 = 2 AND 3";
				//"1 = 2";

			return Solution;
		}
};