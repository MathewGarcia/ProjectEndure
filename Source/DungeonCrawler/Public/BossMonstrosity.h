// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossMonstrosity.generated.h"

class APlayerCharacterState;
class AInteractableBuffActor;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ABossMonstrosity : public AEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Lives")
	int Lives = 3;

	UPROPERTY(EditAnywhere,Category = "Enemies To Spawn")
	TSubclassOf<AEnemy> EnemyToSpawn;

	UPROPERTY(EditAnywhere, Category = "Amount To Spawn")
	int AmountToSpawn = 5;

	UPROPERTY(EditAnywhere, Category = "Radius")
	float Radius = 50.f;
		
	UPROPERTY()
	TArray<AEnemy*> ActiveSludges;

	void SpawnEnemies();

	void HandleDeath(AActor*DamageCauser);

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;

	int currentLives;

	void ResetBoss();

	virtual void ResetEnemy() override;

};
