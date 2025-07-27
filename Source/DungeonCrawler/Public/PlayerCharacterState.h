// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include <GameStats.h>

#include "GearItemInstance.h"
#include "VariableVariant.h"
#include "GearType.h"
#include "ItemData.h"
#include "PlayerCharacterState.generated.h"


/**
 * 
 */

class UQuest;
enum class EAbilityTalentRow : uint8;
class UInputAction;
class UNiagaraSystem;
class UBuff;
class UItemDataObject;
class UAbility;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, const TArray<UItemDataObject*>, Inventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAbilitiesUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponUpdated, AWeapon*, Weapon);

USTRUCT(Blueprintable)
struct DUNGEONCRAWLER_API FAbilitySaveData
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<UAbility> AbilityClass;
	UPROPERTY()
	TMap<FString, FVariableVariant> AbilityMetaData;

};

class UBaseItem;

UCLASS()
class DUNGEONCRAWLER_API APlayerCharacterState : public APlayerState
{
	GENERATED_BODY()

public:

	APlayerCharacterState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	FGameStats playerStats;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int MaxLives;

	UPROPERTY(EditAnywhere,Instanced,BlueprintReadWrite,Category = "Inventory")
	TArray<UItemDataObject*> Inventory;

	UPROPERTY(EditAnywhere,Category = "Gain Life")
	UNiagaraSystem* GainLifeNiagaraSystem;

	UPROPERTY(BlueprintAssignable,Category= "Inventory")
	FOnInventoryUpdated FInventoryUpdated;

	UPROPERTY(BlueprintReadWrite,Category = "Player gear")
	TMap<EGearType, AItem*> PlayerGear;

	UPROPERTY()
	TMap<EGearType, UGearItemInstance*> PlayerGearInstances;


	UPROPERTY(EditAnywhere,Category = "Learned Abilities")
	TArray<UAbility*> LearnedAbilities;

	UPROPERTY()
	TArray<UAbility*>EquippedAbilities;


	UPROPERTY(EditAnywhere,Category  = "Heal Ability")
	TSubclassOf<UAbility> HealAbility;

	UPROPERTY(EditAnywhere,Category = "Stamina Rate Amount")
	float StaminaRate = 5.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LevelUpNiagara")
	UNiagaraSystem* LevelUpNiagaraSystem;

	UPROPERTY()
	TArray<UQuest*> CurrentQuests;

	FOnAbilitiesUpdated OnAbilitiesUpdated;

	FOnWeaponUpdated OnWeaponUpdated;

	virtual void BeginPlay() override;


	void updateStrength(float strengthAmt);

	void updateStamina(float staminaAmt);

	void updateHealth(float healthAmt);

	void updateIntellect(float intellectAmt);

	void updateMana(float manaAmt);

	float GetTotalHealth()
	{
		return playerStats.totalHealth;
	}

	float GetTotalStamina()
	{
		return playerStats.totalStamina;
	}

	float GetTotalMana()
	{
		return playerStats.totalMana;
	}

	float GetTotalStrength()
	{
		return playerStats.totalStrength;
	}

	float GetTotalIntellect()
	{
		return playerStats.totalIntellect;
	}

	void AddItemToInventory(UItemDataObject* item);

	void RemoveItemFromInventory(UItemDataObject* item);


	void RemoveItemFromInventory(UItemDataObject* item,int Amount);

	TArray<UItemDataObject*> GetInventory()
	{
		return Inventory;
	}

	UPROPERTY()
	TArray<UBuff*> PassiveBuffs;

	TArray<FAbilitySaveData> LearnedAbilitiesSaveData;


	void SerializeObject(UObject*Object,APlayerCharacterState*NewPCS);

	virtual void CopyProperties(APlayerState* PlayerState) override;

	void SaveAbilities(APlayerCharacterState*NewPCS);

	void DeserializeAbilities();

	float DamageMultiplier = 1.f;


	void UpdateLives(int amount);

	int GetCurrentLives()
	{
		return currentLives;
	}

	void SetCurrentLives(int amount);

	UFUNCTION()
	void PlayerLevelUp(const int& level);

	TMap<FString, FKey> InputMap;

	void ReBindDelegates();

	bool bInDungeon = false;

private:

	bool bSaved = false;

	int currentLives;

	//TODO recalculate stats after level up. calulateTotalStat works in that regard, however, we don't have a function that updates the stat in question. (probably will be on the character)

};
