// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "ItemDataObject.h"
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CurrentWeaponLevel = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int WeaponLevel = 0;  // Add this if it's different from CurrentWeaponLevel

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Strength = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Intellect = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int BaseDamage = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EElementTypes WeaponElementType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ElementTypeDamage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int IronCoreResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int IronFragmentResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int IronStoneResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int ElementalCoreResourceAmt = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int EXPNeeded = 0;

	UPROPERTY()
	TMap<UItemDataObject*, int> ItemsToRemove;

	void UpgradeWeapon();
	void InitAccordingToLevel(int Level);
	bool bCanUpgrade();
	void SerializeWeapon(FWeaponInstanceSaveData& Output);
	void RandomlySetElementType();
};