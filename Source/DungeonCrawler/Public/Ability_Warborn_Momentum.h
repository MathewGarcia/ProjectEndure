// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Warborn_Momentum.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Warborn_Momentum : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float AmtPercent = 0.05;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle FCooldown;

	FDelegateHandle FOnDamageDelegateHandle;

};
