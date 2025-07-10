// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Flow_Barrier.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Flow_Barrier : public UAbility
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Radius")
	float Radius = 1000.f;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle FCooldown;
};
