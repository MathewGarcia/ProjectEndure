#pragma once
#include "CoreMinimal.h"
#include "Ability_Talent_Row_Enum.h"
#include "VariableVariant.generated.h"
USTRUCT(Blueprintable)
struct DUNGEONCRAWLER_API FVariableVariant
{
	GENERATED_BODY()

	UPROPERTY()
	bool BoolVar = false;

	UPROPERTY()
	float FloatVar = 0.f;

	UPROPERTY()
	int IntVar = 0;

	UPROPERTY()
	FString StringVar = "";

	UPROPERTY()
	FName NameVar = "";

	UPROPERTY()
	EAbilityTalentRow AbilityTalentRow;
};