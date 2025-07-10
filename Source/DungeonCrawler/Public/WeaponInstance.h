// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDataObject.h"
#include "UObject/NoExportTypes.h"
#include "WeaponInstance.generated.h"

struct FWeaponInstanceSaveData;
enum class EElementTypes : uint8;
class UWeaponItemData;
/**
 * 
 */
UCLASS(Blueprintable, EditInlineNew)
class DUNGEONCRAWLER_API UWeaponInstance : public UItemDataObject
{
	GENERATED_BODY()

public:

	int CurrentWeaponLevel = 0;
	int Strength = 0;
	int Intellect = 0;
	int BaseDamage = 0;
	EElementTypes WeaponElementType;
	float ElementTypeDamage;

	void UpgradeWeapon();

	void InitAccordingToLevel(int Level);

	int IronCoreResourceAmt = 0;

	int IronFragmentResourceAmt = 0;

	int IronStoneResourceAmt = 0;

	int ElementalCoreResourceAmt = 0;

	int EXPNeeded = 0;

	UPROPERTY()
	TMap<UItemDataObject*,int> ItemsToRemove;
	bool bCanUpgrade();

	void SerializeWeapon(FWeaponInstanceSaveData& Output);

	void RandomlySetElementType();
};
