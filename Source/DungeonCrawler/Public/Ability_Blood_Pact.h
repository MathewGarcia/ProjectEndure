// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Blood_Pact.generated.h"

class AEnemy;
class UNiagaraSystem;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Blood_Pact : public UAbility
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Niagara System To Play")
	UNiagaraSystem* NiagaraSystemToPlay;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	TWeakObjectPtr<UNiagaraComponent> NC;

private:
	FTimerHandle FCooldown;

	bool bActivated = false;

	FDelegateHandle FBloodpactDelegate;

	UPROPERTY()
	AEnemy* LinkedEnemy;

	float DamageRecieved = 0.f;
	
};
