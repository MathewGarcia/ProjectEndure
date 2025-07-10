// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "ShadowStabAbility.generated.h"

class ADebuffActor;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UShadowStabAbility : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Actor to spawn")
	TSubclassOf<ADebuffActor> ActorToSpawn;

	UPROPERTY(EditAnywhere,Category = "Cooldown Timer")
	float CooldownTimer;

	FTimerHandle FCooldownTimer;
	virtual bool bShouldExecute_Implementation() override;

	virtual void execute_Implementation() override;

	virtual void Logic() override;
};
