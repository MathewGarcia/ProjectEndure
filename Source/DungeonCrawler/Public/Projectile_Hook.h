// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "CableComponent.h"
#include "Projectile_Hook.generated.h"

class UAbility_Hook;
DECLARE_DELEGATE_OneParam(FOnHookedActor, AActor*);
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

	// Delegate to notify when a hook lands
	FOnHookedActor OnHookedActor;

	UPROPERTY()
	bool bIsReturning = false;

	void ReturnHookProjectile();

protected:
	virtual void BeginPlay() override;

private:

    UPROPERTY(EditAnywhere, Category = "Chain Cable")
    UCableComponent* Cable;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Chain Cable")
    UStaticMesh* ChainMesh;

    UPROPERTY(EditDefaultsOnly, Category = "Chain Cable")
    UMaterialInterface* ChainMaterial;
};
