// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Projectile.h"
#include "AbilityRootedFire.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityRootedFire : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Sphere Radius")
	float SphereRadius;

	UPROPERTY(EditAnywhere,Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileToSpawn;

	UPROPERTY(EditAnywhere,Category = "Projectile")
	int amountToSpawn;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float timeToLaunch;

	UPROPERTY()
	TArray<AProjectile*> Projectiles;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;
};
