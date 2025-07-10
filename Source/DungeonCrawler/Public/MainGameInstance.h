// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "ElementTypes.h"
#include "GameStats.h"
#include "PlayerCharacterState.h"
#include "QuestDataAsset.h"
#include "Quest_SaveData.h"
#include "WeaponInstanceSaveData.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDungeonGenerationComplete);

UENUM()
enum class ELoadType
{
	SaveGame UMETA(DisplayName = "SaveGame"),
	GameInstance UMETA(DisplaName = "GameInstance"),
	Tutorial UMETA(DisplayName = "Tutorial")
};


class UPlayerSaveGame;
class ULiquidDataAsset;
class UNiagaraSystem;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY()
	APlayerCharacter* localPlayer;

	UPROPERTY(EditAnywhere,Category = "Blood")
	TArray<UNiagaraSystem*> BloodSystems;

	UPROPERTY(EditAnywhere,Category = "Debuffs")
	TMap<EElementTypes, TSubclassOf<UElementDebuff>> ElementDebuffs;

	UPROPERTY(EditAnywhere, Category = "LiquidDataAssets")
	TMap<EElementTypes, ULiquidDataAsset*> LiquidDataAssetsMap;

	UPROPERTY(EditAnywhere,Category = "Items Map")
	TMap<FString, UDataAsset*> AllItems;

	UPROPERTY(EditAnywhere,Category = "Quests")
	TMap<FString, UQuestDataAsset*> Quests;

	UPROPERTY(EditAnywhere,Category = "Weapon Element Type")
	TMap<EElementTypes, UNiagaraSystem*> WeaponParticleMap;


	UPROPERTY(EditAnywhere, Category = "Elemental Damage Map")
	TMap<EElementTypes, UNiagaraSystem*> ElementalDamageMap;


	void SaveGameData();


	void LoadGameData();

	UPlayerSaveGame* GetSavedGame() const { return SavedGame; }

	void DeleteGameData();

	bool bLoadingSucceeded = false;

	UPROPERTY()
	FGameStats GIPlayerStats;

	UPROPERTY(VisibleAnywhere, Category = "LearnedAblities")
	TArray<FAbilitySaveData> GILearnedAbilities;

	UPROPERTY(VisibleAnywhere, Category = "Player Gear")
	FWeaponInstanceSaveData GICurrentWeapon;

	UPROPERTY(VisibleAnywhere, Category = "Player Gear")
	TArray<FGearItemInstanceSaveData> GICurrentGear;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FItemInstance> GIInventory;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FWeaponInstanceSaveData> GIInventoryWeapons;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TArray<FGearItemInstanceSaveData> GIInventoryGear;

	UPROPERTY(VisibleAnywhere, Category = "HealAbility")
	TSubclassOf<UAbility> GIHealAbility;

	UPROPERTY(EditAnywhere, Category = "Wood")
	TArray<USoundBase*> Wood;

	UPROPERTY(EditAnywhere, Category = "Cobble")
	TArray<USoundBase*> Cobble;

	UPROPERTY(EditAnywhere, Category = "Grass")
	TArray<USoundBase*> Grass;

	UPROPERTY(EditAnywhere, Category = "Hit")
	TArray<USoundBase*>Hit;

	UPROPERTY(EditAnywhere, Category = "Liquid")
	TArray<USoundBase*> Liquid;

	UPROPERTY(EditAnywhere, Category = "Environment")
	TArray<USoundBase*> EnvironmentalItemBreaks;


	UPROPERTY(EditAnywhere, Category = "Environment")
	TArray<USoundBase*> WingSounds;

	UPROPERTY(EditAnywhere, Category = "Dodge")
	USoundBase* DodgeSound;

	UPROPERTY(EditAnywhere,Category = "PickUpSound")
	USoundBase* PickUpSound;

	UPROPERTY(EditDefaultsOnly)
	USoundAttenuation* Attenuation;

	UPROPERTY(EditDefaultsOnly,Category = "LevelUpSound")
	USoundBase* LevelUpSound;

	UPROPERTY()
	int GIRound = 0;

	UPROPERTY()
	int GICurrentLives = 0;

	UPROPERTY()
	TArray<FQuest_SaveData> GICurrentQuestSaveData;

	ELoadType LoadType;

	void UpdateInDungeon(bool bInDungeon);

	void PlayDodgeSound(AActor* Actor);

	FOnDungeonGenerationComplete OnDungeonGenerationComplete;

	bool bGamePaused = false;

protected:
	virtual void Init() override;

	UPROPERTY()
	UPlayerSaveGame* SavedGame;

};
