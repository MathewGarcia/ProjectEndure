// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Blood_Frenzy.generated.h"

class UNiagaraSystem;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Blood_Frenzy : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Sound")
	USoundBase* SoundToPlay;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> WidgetToShow;

	UPROPERTY(EditAnywhere, Category = "Widget")
	UNiagaraSystem* NiagaraSystem;

	void PlayWidget(APlayerCharacter* Player);

	int KillCount = 0;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	UPROPERTY(EditAnywhere,Category = "Kills Needed To Activate")
	int KillsNeeded = 10;

	UPROPERTY(EditAnywhere,Category = "Duration")
	float Duration = 15.f;

	void PlaySound();

	void PlayNiagaraSystem(APlayerCharacter* player);
};
