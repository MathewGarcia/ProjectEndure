// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Elemental_Barrage.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Elemental_Barrage : public UAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Orb Spawn Rate")
	float SpawnRate;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle FCooldown;

	FTimerHandle FFireRate;

	int MaxToSpawn = 5;

	int currentAmount = 0;

};
