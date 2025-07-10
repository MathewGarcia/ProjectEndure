// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Bursting_Shield.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Bursting_Shield : public UAbility
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

	float Shield = 0.f;

	bool bActivated = false;

	FDelegateHandle FShieldDelegateHandle;

};
