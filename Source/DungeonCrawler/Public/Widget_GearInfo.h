// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_GearInfo.generated.h"

class UHorizontalBox;
class UItemDataObject;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UWidget_GearInfo : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WeaponHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* GearHB;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentWeaponLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentBaseDamage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentStrength;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentIntellect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentElement;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentElementDamage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearStamina;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearStrength;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearIntellect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearPhysicalProtection;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearPoiseProtection;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearWaterResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearFireResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearLightningResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearShadowResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearBleedResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentGearPoisonResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RequiredStrength;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RequiredIntellect;

public:
	void SetInfo(UItemDataObject* ItemDataObject);
	
};
