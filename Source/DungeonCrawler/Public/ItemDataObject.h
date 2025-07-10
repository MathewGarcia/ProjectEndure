// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemInstance.h"
#include "ItemDataObject.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDataChanged);

UCLASS(Blueprintable,EditInlineNew)
class DUNGEONCRAWLER_API UItemDataObject : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UItemDataAsset* ItemData;

	UPROPERTY()
	int32 Quantity;

	UPROPERTY()
	FOnItemDataChanged OnItemDataChanged;

	UFUNCTION()
	void SetQuantity(int32 NewQuantity);

};
