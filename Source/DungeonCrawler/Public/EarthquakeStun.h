// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "EarthquakeStun.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UEarthquakeStun : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere, Category = "Niagara To Play")
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, Category = "Sound To Play")
	USoundBase* SoundToPlay;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	FTimerHandle FCooldown;

	FTimerHandle FFakeTick;

	TArray<FTimerHandle> Timers;
};
