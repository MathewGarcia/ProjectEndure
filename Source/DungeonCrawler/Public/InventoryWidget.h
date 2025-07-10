// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UItemDataObject;
class UInventoryItemWidget;
class UTileView;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere,Blueprintable,Category="InventoryWidgetClass")
	TSubclassOf<UInventoryItemWidget> InventoryItemWidgetClass;
protected:

	UPROPERTY(meta=(BindWidget))
	UTileView* InventoryTileView;


	FTimerHandle updateItemInfoTimer;

	UFUNCTION()
	void UpdateInventory(TArray<UItemDataObject*> Inventory);

	

	virtual void NativeConstruct() override;
	
};
