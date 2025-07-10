// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "AbilityThrowBoulder.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityThrowBoulder : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileToSpawn;

	
	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle ThrowTimer;

	FTimerHandle FCooldown;
};
