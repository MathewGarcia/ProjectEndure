// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "WeaponItemData.h"
#include "WeaponItem.generated.h"

/**
 * 
 */
class AWeapon;
UCLASS()
class DUNGEONCRAWLER_API UWeaponItem : public UBaseItem
{
	GENERATED_BODY()
	
public:
	virtual void useItem_Implementation(APlayerCharacter* player) override;
};
