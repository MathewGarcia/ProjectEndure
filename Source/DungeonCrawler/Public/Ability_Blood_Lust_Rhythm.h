// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Blood_Lust_Rhythm.generated.h"

class UNiagaraSystem;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Blood_Lust_Rhythm : public UAbility
{
	GENERATED_BODY()



	UPROPERTY(EditAnywhere, Category = "Duration")
	float Duration;

	UPROPERTY(EditAnywhere, Category = "Duration")
	int KillsNeeded = 3;

	UPROPERTY(EditAnywhere, Category = "Duration")
	float AttackSpeedModifierPercent = 0.5;

	UPROPERTY(EditAnywhere, Category = "Duration")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> WidgetToShow;
	
	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	void PlaySound(UWorld*playerWorld);

	void PlayWidget(APlayerCharacter* Player);

	UPROPERTY(EditAnywhere, Category = "Widget")
	UNiagaraSystem* NiagaraSystem;

	void PlayNiagaraSystem(APlayerCharacter* player);


private:
	FTimerHandle FCooldown;

	FDelegateHandle FBloodLustRhythmDelegate;

	FTimerHandle FDurationTimer;

	int CurrentAmtKills = 0;


};
