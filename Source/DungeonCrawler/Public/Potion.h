// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "Potion.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UPotion : public UBaseItem
{
	GENERATED_BODY()
	
public:


	UPROPERTY(EditAnywhere,Category = "Time")
	float time;

	virtual void useItem_Implementation(APlayerCharacter*player) override;


	FTimerHandle BuffTimerHandle;

};
