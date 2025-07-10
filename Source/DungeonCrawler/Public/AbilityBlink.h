// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "AbilityBlink.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbilityBlink : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Sound To Play")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, Category = "Sound To Play")
	UNiagaraSystem* NiagaraSystem;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	FTimerHandle FCooldown;

	void PlaySound(AActor* Actor);

	void PlayNiagaraSystem(AActor*Actor);
};
