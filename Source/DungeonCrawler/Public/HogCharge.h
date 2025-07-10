// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "HogCharge.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UHogCharge : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Rage Increase Rate Amount")
	float RageAmount = 0.f;

	UPROPERTY(EditAnywhere, Category = "Rage Increase Rate Time")
	float RageTime = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Rage Increase Rate Time")
	float CooldownTime = 15.f;

	UPROPERTY(EditAnywhere,Category = "Sound To Play")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, Category = "Anim Montage To Play")
	UAnimMontage* MontageToPlay;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	void PlaySound();

private:
	FTimerHandle FRageTimerHandle;

	FTimerHandle FRageTimerCooldown;

	float Rage = 0.f;

	void HandleRageLogic();

};
