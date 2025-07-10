// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "AbilityRainingArrows.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityRainingArrows : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileToSpawn;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	int amountToSpawn;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float CircleRadius = 300.f;


	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	FTimerHandle FCooldown;
};
