// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class AEnemy;


UENUM()
enum class ESpawnPointState : uint8
{
	none UMETA(DisplayName = "none"),
	fakeDead UMETA(DisplayName = "fakeDead"),
	fakeAttack UMETA(DisplayName = "fakeAttack")
};

UCLASS()
class DUNGEONCRAWLER_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

	UPROPERTY(EditAnywhere,Category = "Spawn point mesh")
	UStaticMeshComponent* SpawnPointMesh;


	virtual AEnemy* SpawnEnemy();

	UPROPERTY(EditAnywhere,Category = "Spawnpoint state")
	ESpawnPointState SpawnPointState;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category = "SpawnTable")
	TMap<TSubclassOf<AEnemy>, float> EnemySpawnTable;


};
