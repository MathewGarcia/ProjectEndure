// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Elemental_Beam.generated.h"

class UNiagaraSystem;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Elemental_Beam : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;

	UPROPERTY(EditAnywhere,Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere,Category = "NiagaraSystemToPlay")
	UNiagaraSystem* NiagaraSystem;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	virtual void Logic() override;

	void ChannelBeam(APlayerCharacter*player);

private:
	FTimerHandle FCooldown;
	FTimerHandle FCustomTick;
	FTimerHandle FDuration;
};
