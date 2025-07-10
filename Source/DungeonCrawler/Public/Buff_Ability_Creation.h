// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff.h"
#include "Buff_Ability_Creation.generated.h"

class UAbility;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBuff_Ability_Creation : public UBuff
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Ability To Spawn")
	TSubclassOf<UAbility> AbilityToSpawn;

	virtual void Execute() override;
};
