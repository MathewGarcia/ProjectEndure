// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "Item.h"
#include "WeaponItemData.h"
#include "GameFramework/Actor.h"
#include "GearWeaponItem.generated.h"

class APlayerCharacter;
class UBoxComponent;
class UBaseItem;
UCLASS()
class DUNGEONCRAWLER_API AGearWeaponItem : public AItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere, Category = "Item information")
	UWeaponItemData* WeaponItemData;




};
