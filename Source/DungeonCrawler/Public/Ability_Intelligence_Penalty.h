// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Intelligence_Penalty.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Intelligence_Penalty : public UAbility
{
	GENERATED_BODY()

public:

	int Amount;

	UAbility_Intelligence_Penalty();

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void SerializeObject(FAbilitySaveData& OutData) override;

	virtual void DeserializeObject(const FAbilitySaveData& InData) override;
};
