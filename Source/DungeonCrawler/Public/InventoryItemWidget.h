// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UWidget_GearInfo;
class UItemDataObject;
class UButton;
class UInventoryWidget;
class UImage;
class UTextBlock;
/**
 * 
 */

UCLASS()
class DUNGEONCRAWLER_API UInventoryItemWidget : public UUserWidget , public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere,meta = (BindWidget))
	UTextBlock* QuantityText;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemPic;

	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton;

	UFUNCTION(BlueprintCallable,Category = "QuantityBinding")
	int GetQuantity();

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void RefreshText();


	UFUNCTION()
	void OnItemClicked();

	UFUNCTION()
	void OnItemHovered();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	UItemDataAsset* itemData;

	UPROPERTY()
	UItemDataObject* itemDataObject;

	UPROPERTY()
	UWidget_GearInfo* GearInfoWidget;
};
