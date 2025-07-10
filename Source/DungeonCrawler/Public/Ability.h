// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TalentWidget.h"
#include "VariableVariant.h"
#include "UObject/NoExportTypes.h"
#include "Ability_Talent_Row_Enum.h"
#include "Ability.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAbilityUsed);

enum class EAbilityTalentRow : uint8;
struct FAbilitySaveData;

UENUM(Blueprintable)
enum class EAbilityType	: uint8
{
	Offensive UMETA(Display = "Offesnive"),
	Defensive UMETA(Display = "Defensive")
};

UENUM(Blueprintable)
enum class EAbilityActivationType : uint8
{
	Activate UMETA(Display = "Activate"),
	Timer UMETA(Display = "Timer"),
	Event UMETA(Display = "Event")
};
class UTexture2D;
UCLASS(Blueprintable,Abstract)
class DUNGEONCRAWLER_API UAbility : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Ability information")
	FString AbilityName;

	UPROPERTY(EditAnywhere, Category = "Ability information")
	FString AbilityDescription;

	UPROPERTY(EditAnywhere, Category = "Ability information")
	int manaCost;

	UPROPERTY(EditAnywhere, Category = "Ability information")
	int staminaCost;

	UPROPERTY(EditAnywhere, Category = "Ability information")
	UTexture2D* Image;

	UPROPERTY(EditAnywhere, Category = "Ability information")
	EAbilityType AbilityType;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float Cooldown = 0.f;

	UPROPERTY(EditAnywhere, Category = "Cooldown")
	float MinCooldownTime = 0.f;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Execute Ability")
	void execute();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "check Ability")
	bool bShouldExecute();

	UPROPERTY(EditAnywhere, Category = "Ability Activation Type")
	EAbilityActivationType AbilityActivationType;

	virtual void Logic();

	void SetIsLearned(bool bNewIsLearned);

	virtual void SerializeObject(FAbilitySaveData& OutData);

	virtual void DeserializeObject(const FAbilitySaveData& InData);

	virtual void OnTriggered();

	FAbilityUsed OnAbilityUsed;

	UPROPERTY()
	EAbilityTalentRow AbilityTalentRow = EAbilityTalentRow::None;

protected:
	bool bIsLearned;
};
