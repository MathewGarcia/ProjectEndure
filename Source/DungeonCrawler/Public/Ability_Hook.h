// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Hook.generated.h"

class AProjectile_Hook;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Hook : public UAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Cooldown")
	FTimerHandle FCooldown;

	UPROPERTY(EditAnywhere,Category = "AnimMontageToPlay")
	UAnimMontage* AnimMontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Projectile To Spawn")
	TSubclassOf<AProjectile_Hook> ProjectileToSpawn;

	UPROPERTY(EditAnywhere, Category = "Ability Damage")
	float AbilityDamage;

	virtual bool bShouldExecute_Implementation() override;

	virtual void execute_Implementation() override;

	virtual void Logic() override;

	virtual void OnTriggered() override;

};
