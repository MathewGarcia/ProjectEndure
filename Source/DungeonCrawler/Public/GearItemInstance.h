// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataObject.h"
#include "GearItemInstance.generated.h"

struct FGearItemInstanceSaveData;
class UGearItemData;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class DUNGEONCRAWLER_API UGearItemInstance : public UItemDataObject
{
	GENERATED_BODY()
public:

	int Strength = 0;
	int Intellect = 0;
	int GearLevel = 0;
	int Stamina = 0;
	int Health = 0;
	float PhysicalProtection = 0.f;
	float PoiseProtection = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float WaterResistance = 0.0; //slows enemy, if hit with fire the enemy will steam. Burning them causing a dot.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float FireResistance = 0.0; //Burn debuff is added causing the player to become weaker to physical attacks.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float LighteningResistance = 0.0; //cause a lightening strike to come down and stun the player.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float ShadowResistance = 0.0; //Shadow hand appear attempting to strike the enemy.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float BleedResistance = 0.0; //Bleed causing 2x damage.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float PoisonResistance = 0.0; //Damage over time is caused.

	int IronCoreResourceAmt = 0;

	int IronFragmentResourceAmt = 0;

	int IronStoneResourceAmt = 0;

	int ElementalCoreResourceAmt = 0;

	int EXPNeeded = 0;

	UPROPERTY()
	TMap<UItemDataObject*,int> ItemsToRemove;


	void InitializeStats();

	void UpgradeGear();

	bool bCanUpgrade();

	void SeralizeGearItem(FGearItemInstanceSaveData& Output);

	void InitStatsAccordingToLevel(int Level);
};
