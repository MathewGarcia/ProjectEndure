// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Greater_Orb.generated.h"

class AProjectile;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Greater_Orb : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Niagara System To Play")
	UNiagaraSystem* NiagaraSystemToPlay;


	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle FCooldown;
	
};
