// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Infinite_Ablities.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Infinite_Ablities : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = " Kills Needed")
	int KillsNeeded = 3;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;


private:
	FTimerHandle FCooldown;

	FTimerHandle FDuration;

	int CurrentKills;

	FDelegateHandle FInfiniteAbilitiesDelegateHandle;

	
};
