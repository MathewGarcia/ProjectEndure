// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Rooting_Projectile.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Rooting_Projectile : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "NiagaraSystemToPlay")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	UPROPERTY(EditAnywhere,Category = "Max Projectiles")
	int MaxProjectiles = 3;


private:
	FTimerHandle FCooldown;

	FTimerHandle FDuration;

	FDelegateHandle FRootingProjectileDelegate;

	int CurrentProjectileCount = 0;

	bool bActive;

	TWeakObjectPtr<UNiagaraComponent> safeNC;
};
