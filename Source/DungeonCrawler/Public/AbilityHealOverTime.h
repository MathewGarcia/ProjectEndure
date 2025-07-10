// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "AbilityHealOverTime.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityHealOverTime : public UAbility
{
	GENERATED_BODY()


public:


	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Ability Total Time")
	float AbilityTime;

	UPROPERTY(EditAnywhere,Category = "Ability CD time")
	float CooldownTime;

	UPROPERTY(EditAnywhere,Category = "Heal Rate")
	float HealRate = 0.5f;

	UPROPERTY(EditAnywhere,Category = "Heal Amount")
	float HealAmount = 6.f;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	void BeginDestroy() override;

	UPROPERTY()
	UWorld* playerWorld;

	FTimerHandle FCooldownTimer;

	FTimerHandle FHealTimer;

	FTimerHandle FInnerHealTimer;

};
