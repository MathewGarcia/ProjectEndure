// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Infinite_Poise.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Infinite_Poise : public UAbility
{
	GENERATED_BODY()

public:
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
protected:
	FTimerHandle FCooldown;
};
