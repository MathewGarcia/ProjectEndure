// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "BlacksmithItemWidget.generated.h"

class UHorizontalBox;
class UGearItemInstance;
class UItemDataObject;
class UBlacksmithUpgradeUI;
class UTextBlock;
class UItemDataAsset;
class UImage;
class UButton;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpgradeItemClicked, UItemDataAsset*, ItemData);


UCLASS()
class DUNGEONCRAWLER_API UBlacksmithItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (BindWidget))
	UImage* BackgroundImage;


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
	UTextBlock* NewWeaponLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewBaseDamage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewStrength;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewIntellect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewElement;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewElementDamage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;

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
	UTextBlock* NewGearLevel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearHealth;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearStamina;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearStrength;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearIntellect;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearPhysicalResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearPoiseResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearWaterResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearFireResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearLightningResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearShadowResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearBleedResistance;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NewGearPoisonResistance;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* WeaponHB;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* GearHB;

	UPROPERTY()
	UItemDataAsset* ItemData;

	UPROPERTY()
	UItemDataObject* IDO;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IronCoreCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IronFragmentCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* IronStoneCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ElementCost;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EXPCost;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUpgradeItemClicked OnUpgradeItemClicked;

	void SetParentWidget(UBlacksmithUpgradeUI* NewParentWidget);

	void SetGearInformation(UGearItemInstance*GearItemInstance);
private:
	UPROPERTY()
	UBlacksmithUpgradeUI* ParentWidget;


};
