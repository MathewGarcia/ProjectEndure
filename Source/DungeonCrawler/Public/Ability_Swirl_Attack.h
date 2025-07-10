// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Swirl_Attack.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Swirl_Attack : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Animation To Play")
	UAnimMontage* MontageToPlay;


	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	virtual void OnTriggered() override;

private:
	FTimerHandle FCooldown;

};
