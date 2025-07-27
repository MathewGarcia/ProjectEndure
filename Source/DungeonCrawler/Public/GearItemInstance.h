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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	int Strength = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	int Intellect = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	int GearLevel = 0;  // This is especially important for your stacking logic!

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	int Stamina = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	int Health = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	float PhysicalProtection = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Stats")
	float PoiseProtection = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float WaterResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float FireResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float LighteningResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float ShadowResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float BleedResistance = 0.0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float PoisonResistance = 0.0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Resources")
	int IronCoreResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Resources")
	int IronFragmentResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Resources")
	int IronStoneResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Resources")
	int ElementalCoreResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Gear Resources")
	int EXPNeeded = 0;

	UPROPERTY()
	TMap<UItemDataObject*, int> ItemsToRemove;

	void InitializeStats();
	void UpgradeGear();
	bool bCanUpgrade();
	void SeralizeGearItem(FGearItemInstanceSaveData& Output);
	void InitStatsAccordingToLevel(int Level);
};