// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnPoint.h"

#include "Enemy.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	SpawnPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawn Point Mesh"));
	SpawnPointMesh->SetHiddenInGame(true);
	RootComponent = SpawnPointMesh;


}

AEnemy* ASpawnPoint::SpawnEnemy()
{
	float TotalWeight = 0.f;
	for(const auto&pair : EnemySpawnTable)
	{
		TotalWeight += pair.Value;
	}

	float Roll = FMath::FRandRange(0.f, TotalWeight);
	TSubclassOf<AEnemy> EnemyChosen = nullptr;
	for (const auto& pair : EnemySpawnTable)
	{
		Roll -= pair.Value;

		if(Roll <= 0.f)
		{
			EnemyChosen = pair.Key;
			break;
		}
	}

	if (EnemyChosen) {
		FActorSpawnParameters params;
		params.Owner = this;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if(AEnemy*SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyChosen, GetActorLocation(), GetActorForwardVector().Rotation(), params))
		{
			switch (SpawnPointState)
			{
			case ESpawnPointState::fakeDead: SpawnedEnemy->EnemyState = EEnemyStates::fakeDead;
				break;

			case ESpawnPointState::fakeAttack: SpawnedEnemy->EnemyState = EEnemyStates::idleAttack;
				break;
			default:break;
			}
			return SpawnedEnemy;
		}
	}

	return nullptr;
}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	SpawnEnemy();

}
