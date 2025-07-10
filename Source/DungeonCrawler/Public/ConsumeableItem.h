// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsumeableItemData.h"
#include "Item.h"
#include "ItemData.h"
#include "GameFramework/Actor.h"
#include "ConsumeableItem.generated.h"

class APlayerCharacter;
class UBoxComponent;
class UBaseItem;
UCLASS()
class DUNGEONCRAWLER_API AConsumeableItem : public AItem
{
	GENERATED_BODY()
	
public:	



	UPROPERTY(EditAnywhere, Category = "Item information")
	UItemDataAsset* ConsumeableItemData;



};
