// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Engine/TimerHandle.h"
#include "Templates/SubclassOf.h"
#include "AoE_Heal.generated.h"

class AAoEHealActor;
class UAnimMontage;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAoE_Heal : public UAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Montage To Play")
	UAnimMontage* MontageToPlay;


	UPROPERTY(EditAnywhere,Category = "HealingRate")
	float HealingRate;


	UPROPERTY(EditAnywhere, Category = "HealingAmount")
	float HealingAmount;

	UPROPERTY(EditAnywhere, Category = "Life cycle")
	float LifeCycle = 15.f;

	UPROPERTY(EditAnywhere,Category = "CooldownTime")
	float CooldownTime;

	UPROPERTY(EditAnywhere, Category = "AOE To Spawn")
	TSubclassOf<AAoEHealActor>ActorToSpawn;

	FTimerHandle FCooldownTimer;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;


	virtual void Logic() override;

};
