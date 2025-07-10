// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Boss_Wave.generated.h"

/**
 * 
 */

UCLASS()
class DUNGEONCRAWLER_API ABoss_Wave : public AEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Type of Enemies")
	TMap<TSubclassOf<AEnemy>,float> EnemiesClasses;

	UPROPERTY(EditAnywhere,Category = "Map Amount To Spawn")
	TMap<TSubclassOf<AEnemy>, int32> AmountMap;

	UPROPERTY(EditAnywhere,Category = "SpawnRate")
	float SpawnRateTime;

	UPROPERTY(EditAnywhere, Category = "Range")
	float MaxRange;


	UPROPERTY()
	TArray<AEnemy*> CurrentEnemies;

	void SpawnEnemy();

	void StartBoss();

	virtual void Destroyed() override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	TSubclassOf<AEnemy> RollForEnemy();


protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

private:
	FTimerHandle FSpawnRateTimerHandle;

	TSubclassOf<AEnemy> ChosenEnemy;

	int32 amountToSpawn = 1;

};
