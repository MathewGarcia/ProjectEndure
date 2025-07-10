// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EquipmentUIWidget.generated.h"

class UGearSlotWidget;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UEquipmentUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UGearSlotWidget* HeadGearSlot;

	UPROPERTY(meta = (BindWidget))
	UGearSlotWidget* RHandGearSlot;

	UPROPERTY(meta = (BindWidget))
	UGearSlotWidget* LHandGearSlot;

	UPROPERTY(meta = (BindWidget))
	UGearSlotWidget* LegsGearSlot;

	UPROPERTY(meta = (BindWidget))
	UGearSlotWidget* ChestGearSlot;
};
