// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "GameFramework/Actor.h"
#include "DungeonPieceActor.generated.h"

class ALiquidActor;
class AEnvironmentalItem;
class ASpawnPoint;
class AEnemy;
enum class ECardinalDirection : uint8;
class UArrowComponent;

UENUM()
enum class EPieceType : uint8
{
	Grass UMETA(DisplayName = "Grass"),
	Cobble UMETA(DisplayName = "Cobble"),
	Crypt UMETA(DisplayName = "Crypt"),
	House UMETA(DisplayName = "House")
};


UENUM()
enum class ELiquidType : uint8
{
	Lava UMETA(DisplayName = "Lava"),
	Water UMETA(DisplayName = "Water"),
	Poison UMETA(DisplayName = "Poison"),
	Blood UMETA(DisplayName = "Blood")
};

UCLASS()
class DUNGEONCRAWLER_API ADungeonPieceActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonPieceActor();

	UPROPERTY(EditAnywhere,Category = "PieceType")
	EPieceType PieceType;

	UPROPERTY(EditAnywhere, Category = "Piece Information")
	int floors = 1;


	UPROPERTY(EditAnywhere,Category = "NorthWall")
	FComponentReference NorthWall;

	UPROPERTY(EditAnywhere,Category = "SouthWall")
	FComponentReference SouthWall;

	UPROPERTY(EditAnywhere,Category = "EastWall")
	FComponentReference EastWall;

	UPROPERTY(EditAnywhere,Category = "WestWall")
	FComponentReference WestWall;

	UPROPERTY(EditAnywhere, Category = "Floor")
	FComponentReference FloorRef;

	UPROPERTY(EditAnywhere,Category = "SpawnPoint")
	USceneComponent* ItemSpawnPoint;


	UPROPERTY(EditAnywhere,Category = "Decorative")
	FComponentReference NorthSocketRef;

	UPROPERTY(EditAnywhere, Category = "Decorative")
	FComponentReference SouthSocketRef;

	UPROPERTY(EditAnywhere, Category = "Decorative")
	FComponentReference EastSocketRef;

	UPROPERTY(EditAnywhere, Category = "Decorative")
	FComponentReference WestSocketRef;

	UPROPERTY(EditAnywhere,Category = "SpawningClass")
	TMap<TSubclassOf<AActor>,float> SpawningEnemies;


	UPROPERTY(EditAnywhere, Category = "SpawningClass")
	TArray<TSubclassOf<AItem>> SpawningItems;


	UPROPERTY(EditAnywhere, Category = "SpawningClass")
	TMap<TSubclassOf<AActor>,float> CornerObjectsMap;

	UPROPERTY(EditAnywhere, Category = "SpawningClass")
	TMap<TSubclassOf<AActor>,float> DecorativeWallActorsMap;


	UPROPERTY(EditAnywhere, Category = "Material Map")
	TMap<ELiquidType, UMaterialInstance*> MaterialMap;


	void HideWall(ECardinalDirection WallToHide,bool bHide);

	UPROPERTY()
	TArray<UStaticMeshComponent*>Walls;

	UPROPERTY()
	TMap<ECardinalDirection,UStaticMeshComponent*> HiddenWalls;

	void AssignMaterial();

	void ActivateWall(UStaticMeshComponent*WallMesh);

	void SetFloorType(const int& FloorType);

	UPROPERTY()
	AActor* SpawnedActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "LiquidTile")
	TSubclassOf<ALiquidActor> LiquidFloorToSpawn;

	int TileHeight = 0;

	void SetFloorMesh(const int& RolledNum);

	FTransform GetWallTransform();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ELiquidType Liquid= ELiquidType::Water;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
