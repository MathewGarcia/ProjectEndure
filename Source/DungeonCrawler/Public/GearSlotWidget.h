// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearType.h"
#include "Blueprint/UserWidget.h"
#include "ItemData.h"
#include "GearSlotWidget.generated.h"

class UItemDataObject;
class UButton;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UGearSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UImage* ItemImage;

	UPROPERTY(meta=(BindWidget))
	UButton* GearButton;

	UPROPERTY()
	UItemDataAsset* itemData;

	UPROPERTY()
	UItemDataObject* itemDataObject;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Slot Type")
	EGearType ItemType;

	void UpdateGearSlot(UItemDataObject* newItemData);

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ResetGearSlot();
};
