// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Projectile_Hook.generated.h"

class UAbility_Hook;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AProjectile_Hook : public AProjectile
{
	GENERATED_BODY()

	virtual void OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY()
	AActor* HookedPlayer;

	virtual void Tick(float DeltaSeconds) override;

public:
	UPROPERTY()
	UAbility_Hook* OwningAbility;

	UPROPERTY()
	AActor* OwningActor;

	void ReturnHookProjectile();


};
