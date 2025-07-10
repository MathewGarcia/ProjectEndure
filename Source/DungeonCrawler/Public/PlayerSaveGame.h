// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStats.h"
#include "GearItemInstanceSaveData.h"
#include "PlayerCharacterState.h"
#include "WeaponInstanceSaveData.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveGame.generated.h"

struct FQuest_SaveData;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere,Category = "GameStats")
	FGameStats playerStats;

	UPROPERTY(VisibleAnywhere,Category = "LearnedAblities")
	TArray<FAbilitySaveData> LearnedAbilities;

	UPROPERTY(VisibleAnywhere, Category = "Player Gear")
	FWeaponInstanceSaveData SavedWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Player Gear")
	TArray<FGearItemInstanceSaveData> SavedGear;

	UPROPERTY(VisibleAnywhere, Category = "Player Gear")
	TArray<FItemInstance> SavedInventory;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FWeaponInstanceSaveData> SavedInventoryWeapons;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FGearItemInstanceSaveData> SavedInventoryGear;

	UPROPERTY(VisibleAnywhere,Category = "HealAbility")
	TSubclassOf<UAbility> HealAbility;

	UPROPERTY(VisibleAnywhere, Category = "PlayerRound")
	int Round = 0;
	
	UPROPERTY()
	int Lives;

	UPROPERTY()
	bool bInDungeon;

	UPROPERTY()
	TArray<FQuest_SaveData> CurrentQuestSaveData;

};
