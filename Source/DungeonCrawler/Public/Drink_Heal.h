// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Drink_Heal.generated.h"

class UAnimMontage;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UDrink_Heal : public UAbility
{
	GENERATED_BODY()
	

public:

	UPROPERTY(EditAnywhere,Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	FTimerHandle FCooldownTimer;

	UPROPERTY(EditAnywhere,Category = "Amount To Heal")
	float HealAmount;


	UPROPERTY(EditAnywhere, Category = "Cooldown Time")
	float CooldownTime;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;


	virtual void Logic() override;

};
