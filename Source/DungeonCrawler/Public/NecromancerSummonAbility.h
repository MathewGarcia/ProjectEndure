// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "NecromancerSummonAbility.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UNecromancerSummonAbility : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Spawn probability Map")
	TMap<TSubclassOf<AEnemy>, float> EnemySpawnMap;

	UPROPERTY(EditAnywhere,Category = "Amount to spawn")
	int amountToSpawn = 0.f;

	UPROPERTY(EditAnywhere, Category = "Distance")
	float Distance = 400.f;

	virtual bool bShouldExecute_Implementation() override;

	virtual void execute_Implementation() override;

	virtual void Logic() override;

	TSubclassOf<AEnemy> EnemyToSpawn();

	FTimerHandle FSummonCoolDownTimer;


	TArray<AEnemy*> Enemies;
};
