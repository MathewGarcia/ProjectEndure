// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.h"
#include "ItemDataObject.h"
#include "BaseItem.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS(Blueprintable,Abstract)
class DUNGEONCRAWLER_API UBaseItem : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	UItemDataAsset* ItemData;

	UFUNCTION(BlueprintCallable,BlueprintNativeEvent, Category = "Item")
	void useItem(APlayerCharacter* player);

	UPROPERTY()
	UItemDataObject* ItemDataObject;


};
