// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Floating_Mine.generated.h"

class AProjectile;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Floating_Mine : public UAbility
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float DurationTimer;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

private:
	FTimerHandle FCooldown;
	FTimerHandle FExplosionTimer;
	TWeakObjectPtr<AProjectile>SafeProjectile;
};
