// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponInstance.h"
#include "WeaponInstanceSaveData.generated.h"

enum class EElementTypes : uint8;
/**
 * 
 */
USTRUCT()
struct DUNGEONCRAWLER_API FWeaponInstanceSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString WeaponID;
	UPROPERTY()
	int CurrentWeaponLevel = 0;
	UPROPERTY()
	int Strength = 0;
	UPROPERTY()
	int Intellect = 0;
	UPROPERTY()
	int BaseDamage = 0;
	UPROPERTY()
	EElementTypes WeaponElementType;
	UPROPERTY()
	float ElementTypeDamage = 0;

	UPROPERTY()
	int Quantity;

	UPROPERTY()
	int IronCoreResourceAmt = 0;

	UPROPERTY()
	int IronFragmentResourceAmt = 0;

	UPROPERTY()
	int IronStoneResourceAmt = 0;

	UPROPERTY()
	int ElementalCoreResourceAmt = 0;

	UPROPERTY()
	int EXPNeeded = 0;

	void InitWeapon(UWeaponInstance*WeaponInstance)
	{
		WeaponInstance->BaseDamage = BaseDamage;
		WeaponInstance->ElementTypeDamage = ElementTypeDamage;
		WeaponInstance->Intellect = Intellect;
		WeaponInstance->CurrentWeaponLevel = CurrentWeaponLevel;
		WeaponInstance->Strength = Strength;
		WeaponInstance->WeaponElementType = WeaponElementType;
		WeaponInstance->IronCoreResourceAmt = IronCoreResourceAmt;
		WeaponInstance->IronFragmentResourceAmt = IronFragmentResourceAmt;
		WeaponInstance->IronStoneResourceAmt = IronStoneResourceAmt;
		WeaponInstance->ElementalCoreResourceAmt = ElementalCoreResourceAmt;
		WeaponInstance->EXPNeeded = EXPNeeded;
		WeaponInstance->SetQuantity(Quantity);
	}
};
