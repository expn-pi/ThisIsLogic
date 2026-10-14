#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "LevelRow.generated.h"

USTRUCT()
struct FLevelRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	FString Statement;

	UPROPERTY(VisibleAnywhere)
	FString Formula;

	UPROPERTY(VisibleAnywhere)
	TArray<FString> Sentences;

	UPROPERTY(VisibleAnywhere)
	FString Solution;
};