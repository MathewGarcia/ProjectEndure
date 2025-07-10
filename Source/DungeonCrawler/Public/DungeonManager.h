// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CardinalDirections.h"
#include "DungeonDecorationStruct.h"
#include "DungeonManager.generated.h"

class ANPC_QuestGiver;
class ALevelMusicActor;
class AEnemy;
class APlayerCharacter;
class AEnvironmentalItem;
class AOrbTeleporter;
class UArrowComponent;
enum class EPieceType : uint8;
class ADungeonPieceActor;

UCLASS()
class DUNGEONCRAWLER_API ADungeonManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonManager();


	UPROPERTY(EditAnywhere, Category = "Dungeon Piece To Spawn")
	TSubclassOf<ADungeonPieceActor> DungeonPieceClass;

	UPROPERTY(EditAnywhere,Category = "Main Music Actor")
	TWeakObjectPtr<ALevelMusicActor> MainMusicActor;

	UPROPERTY(EditAnywhere, Category = "Dungeon Piece RNG")
	int AmountToSpawn = 15;

	UPROPERTY(EditAnywhere, Category = "Max Enemies to Spawn")
	int MaxEnemiesToSpawn = 10;

	UPROPERTY(EditAnywhere,Category = "Map Decorations")
	TArray<FDungeonDecorationStruct> Decorations;

	UPROPERTY(EditAnywhere, Category = "Quest Giver")
	TSubclassOf<ANPC_QuestGiver> QuestGiver;

	void InitializeWorld();


	void RemoveInnerWallsOfConnectedTiles();

	UPROPERTY()
	TArray<ADungeonPieceActor*> CurrentDungeonPieces;

	UPROPERTY(EditAnywhere,Category = "Tile Information")
	float TileSize = 1000.0f;                   

	UPROPERTY(EditAnywhere,Category = "Floor Height")
	float FloorHeight = 400.0f;

	FIntVector RollForDirection(ADungeonPieceActor*SelectedDungeonPiece,TArray<int>& NumbersToIgnore, const TMap<FIntVector,ADungeonPieceActor*>& OccupiedLocations);

	UPROPERTY(EditAnywhere,Category = "Boss Teleporter")
	TArray<TSubclassOf<AOrbTeleporter>> Teleporters;

	ADungeonPieceActor* GetRandomPiece();


	bool AreTilesNeighbors(const FIntVector& TileA, const FIntVector& TileB);
	


	void DFS(ADungeonPieceActor* CurrentTile, ADungeonPieceActor* GoalTile, TArray<ADungeonPieceActor*>& Path,TSet<ADungeonPieceActor*>& Visited, bool& bSuccess);

	void PlaceTeleporter();

	TArray<ADungeonPieceActor*> GetUnvisitedNeighbors(ADungeonPieceActor* CurrentTile, TSet<ADungeonPieceActor*>& Visited);

	UPROPERTY()
	ADungeonPieceActor* Goal = nullptr;

	UPROPERTY()
	TMap<FIntVector, ADungeonPieceActor*> TileGrid;


	void PathRemoveWalls(const TArray<ADungeonPieceActor*>& Path);


	ECardinalDirection GetWallDirectionToRemove(FIntVector InputDirection);

	ECardinalDirection GetOppositeDirection(const ECardinalDirection& CardinalDirection);



	void ShuffleArray(TArray<ADungeonPieceActor*>& ArrayToShuffle);

	UPROPERTY()
	TArray<ADungeonPieceActor*> AvailableTiles;


	bool DoesHaveFreeNeighbors(ADungeonPieceActor*CurrentTile);

	int OpenNeighborsCount(ADungeonPieceActor* Piece);

	void SeparateRooms();

	bool IsLikelyHallway(const TArray<ADungeonPieceActor*>& Room);

	void FloodFillRoom(ADungeonPieceActor* StartTile, TArray<ADungeonPieceActor*>& Room, TSet<ADungeonPieceActor*>& Visited);

	void MarkDeadendTiles(TArray<ADungeonPieceActor*>& A);

	void Decorate();

	TSubclassOf<AActor> RoleActorToSpawn(const TMap<TSubclassOf<AActor>,float>& Map);

	void SetFloorType(ADungeonPieceActor* DungeonPieceActor,EPieceType NewType);

	void SetRoomTheme();

	UPROPERTY(EditAnywhere,Category = "TreeToSpawn")
	TMap<TSubclassOf<AActor>,float> TreeClass;

	UPROPERTY(EditAnywhere, Category = "Column To Spawn")
	TSubclassOf<AActor> ColumnToSpawn;

	UPROPERTY(EditAnywhere,Category = "FakeWalls")
	int MaxFakeWalls;


	UPROPERTY(EditAnywhere, Category = "Fake wall")
	TSubclassOf<AActor> FakeWallToSpawn;

	UPROPERTY(EditAnywhere, Category = "Doorway")
	TSubclassOf<AActor> DoorwayToSpawn;


	void SetTrees(const TArray<ADungeonPieceActor*>& Room);

	void SetColumns(ADungeonPieceActor*SelectedTile);

	void PlaceFakeWalls();

	//may be scrapped.
	void PlaceDoorway();

	TSubclassOf<AEnvironmentalItem> RollForEnvironmentalItem(TMap<float,TSubclassOf<AEnvironmentalItem>>& Map);


	TArray<ADungeonPieceActor*> FreeTilesArray();

	void SetPlayer();

	void SetRemainingEnemies();

	void SetEnemiesInRooms();

	void SetDeadEndItems();

	TArray<ECardinalDirection> AdjacentSides(ECardinalDirection CurrentDirection);

	void PlaceRandomItems(TArray<ADungeonPieceActor*>& Room);

	UPROPERTY(EditAnywhere,Category = "Random Items To Place")
	TMap<TSubclassOf<AActor>,float> RandomItemsMap;

	UPROPERTY(EditAnywhere, Category = "Floor")
	float FloorTileZOffset;

	UPROPERTY(EditAnywhere,Category = "Floor")
	int maxTileHeight = 3;

	UPROPERTY(EditAnywhere, Category = "Floor")
	TSubclassOf<AEnvironmentalItem> FloorToSpawn;


	UPROPERTY(EditAnywhere, Category = "Material Map")
	TMap<EPieceType, UMaterialInstance*> TileMaterialMap;

	void AssignGroundMaterial(EPieceType Type,AEnvironmentalItem*FloorTile);

	void PlaceFloorTiles();

	void SetRoomTileType();

	void InitScaleEnemies(int RoundCount);


	void ResetDungeon();

	void SetWallMaterials(ADungeonPieceActor* Tile);

	bool bIsTileNextToHallway(ADungeonPieceActor*Tile);

	ADungeonPieceActor* GetNeighboringTile(ADungeonPieceActor* Tile);


	void PlaceQuest();

	void CleanWorld();

	void SetResetTeleporter(AOrbTeleporter* ResetTeleporter);

	void ReactivateTeleporter(AOrbTeleporter* TeleporterToReactivate);


private:


	TArray<TArray<ADungeonPieceActor*>> AllRooms;

	UPROPERTY()
	TArray<ADungeonPieceActor*> DeadEndTiles;

	UPROPERTY()
	APlayerCharacter* player;

	UPROPERTY();
	AOrbTeleporter* Teleporter;

	int CurrentFakeWalls = 0;
	int CurrentEnemies = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
