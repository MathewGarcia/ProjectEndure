// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Quick_Chant.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Quick_Chant : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "NiagaraSystemToPlay")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere,Category = "Kills Needed")
	int KillsNeeded;

	UPROPERTY(EditAnywhere, Category = "Percent")
	float PercentIncrease = 2.f;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

protected:
	TWeakObjectPtr<UNiagaraComponent>safeNC;

private:
	int CurrentKills = 0;
	FDelegateHandle FQuickChantDelegateHandle;
	FTimerHandle FDuration;
};
