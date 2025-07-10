// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "GearItemData.h"
#include "GearItem.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UGearItem : public UBaseItem
{
	GENERATED_BODY()
public:

	virtual void useItem_Implementation(APlayerCharacter* player) override;
};
