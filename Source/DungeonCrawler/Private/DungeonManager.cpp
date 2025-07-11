// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonManager.h"

#include "Ability_Intelligence_Penalty.h"
#include "DungeonCrawlerGM.h"
#include "DungeonPieceActor.h"
#include "Enemy.h"
#include "EnvironmentalItem.h"
#include "LevelMusicActor.h"
#include "MainGameInstance.h"
#include "NPC_QuestGiver.h"
#include "OrbTeleporter.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "RoamingSpawnPoint.h"
#include "TreasureChest.h"
#include "Weapon.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADungeonManager::ADungeonManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}


void ADungeonManager::InitializeWorld()
{

	FActorSpawnParameters params;

	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

	int Random = FMath::RandRange(0, 3);

	if (ADungeonCrawlerGM* DCGM = Cast<ADungeonCrawlerGM>(GetWorld()->GetAuthGameMode()))
	{
		int SafeRound = FMath::Max(1, DCGM->GetRoundCount());
		MaxEnemiesToSpawn += FMath::CeilToInt(MaxEnemiesToSpawn * 0.1) * (SafeRound- 1);
		AmountToSpawn += FMath::CeilToInt(AmountToSpawn * 0.1) * (SafeRound - 1);
	}

	if(ADungeonPieceActor*FirstPiece = GetWorld()->SpawnActor<ADungeonPieceActor>(DungeonPieceClass))
	{
		CurrentDungeonPieces.Add(FirstPiece);
		TileGrid.Add(FIntVector(FirstPiece->GetActorLocation()), FirstPiece);
		AvailableTiles.Add(FirstPiece);
		FirstPiece->TileHeight = FMath::RandRange(0, maxTileHeight);
		FirstPiece->SetFloorType(Random);
		FirstPiece->AssignMaterial();
	}

	while(CurrentDungeonPieces.Num() < AmountToSpawn && AvailableTiles.Num() > 0)
	{
		//select a random piece out of what already has been placed.
		UE_LOG(LogTemp, Warning, TEXT("InitWorld loop"));

	
			if (ADungeonPieceActor* SelectedPiece = AvailableTiles.Last())
			{
				int PrevHeight = SelectedPiece->TileHeight;
				int RawHeight = FMath::RandRange(PrevHeight - 2, PrevHeight + 2);
				RawHeight = FMath::Clamp(RawHeight, 0, maxTileHeight);

				float Bias = FMath::FRand();

				if(RawHeight > PrevHeight && Bias > 0.7)
				{
					RawHeight = FMath::Clamp(RawHeight - 1, 0, maxTileHeight);
				}

				if (!DoesHaveFreeNeighbors(SelectedPiece))
				{
					AvailableTiles.Pop();
					continue;
				}

				TArray<int> IgnoreSides;
				//compute the final direction
				FIntVector FinalDirection = RollForDirection(SelectedPiece, IgnoreSides, TileGrid);

				if (FinalDirection == FIntVector(-99))
				{
					AvailableTiles.Pop();
					continue;
				}
				//spawn it and set it.
				if (ADungeonPieceActor* SpawnedPiece = GetWorld()->SpawnActor<ADungeonPieceActor>(DungeonPieceClass, FVector(FinalDirection), FRotator(0)))
				{
					CurrentDungeonPieces.Add(SpawnedPiece);
					TileGrid.Add(FinalDirection, SpawnedPiece);
					if (!AvailableTiles.Contains(SpawnedPiece)) {
						AvailableTiles.Add(SpawnedPiece);
					}
					SpawnedPiece->SetFloorType(Random);
					SpawnedPiece->TileHeight = RawHeight;
					FIntVector Delta = FinalDirection - FIntVector(SelectedPiece->GetActorLocation());
					Delta /= TileSize;

					ECardinalDirection FromDirection = GetWallDirectionToRemove(Delta);
					ECardinalDirection ToDirection = GetOppositeDirection(FromDirection);

					SelectedPiece->HideWall(FromDirection, true);
					SpawnedPiece->HideWall(ToDirection, true);
				}


			}
		
	}


	Goal = CurrentDungeonPieces.Last();
	PlaceTeleporter();

	RemoveInnerWallsOfConnectedTiles();
	SeparateRooms();
	SetRoomTileType();
	PlaceFloorTiles();
	MarkDeadendTiles(DeadEndTiles);
	Decorate();
	PlaceFakeWalls();
	FTimerHandle FTimer;
	SetEnemiesInRooms();
	SetRemainingEnemies();
	//if the player is holding an intellect penalty buff, activate it here.
	GetWorldTimerManager().SetTimer(FTimer, [this]()
		{
			SetPlayer();

			if (player)
			{
				SetRoomTheme();
				SetDeadEndItems();
				PlaceQuest();
				if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
				{
					for (UAbility* Ability : PCS->LearnedAbilities)
					{
						if (UAbility_Intelligence_Penalty* Ability_Intelligence_Penalty = Cast<UAbility_Intelligence_Penalty>(Ability))
						{
							Ability_Intelligence_Penalty->execute_Implementation();
							break;
						}
					}
				}
			}


			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), CurrentEnemiesArr);
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentalItem::StaticClass(), CurrentEnvironmentalItems);

		}, 0.5f, false);



	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MGI->OnDungeonGenerationComplete.Broadcast();
	}

}


void ADungeonManager::RemoveInnerWallsOfConnectedTiles()
{
	for (const auto& Pair : TileGrid)
	{
		FIntVector Location = Pair.Key;
		ADungeonPieceActor* CurrentTile = Pair.Value;

		// Check all 4 directions
		const TArray<FIntVector> Directions = {
			{1,0,0}, {-1,0,0}, {0,1,0}, {0,-1,0}
		};

		for (const FIntVector& Dir : Directions)
		{
			FIntVector NeighborLoc = Location + (Dir * TileSize);
			ADungeonPieceActor** NeighborPtr = TileGrid.Find(NeighborLoc);

			if (NeighborPtr && *NeighborPtr)
			{
				ADungeonPieceActor* Neighbor = *NeighborPtr;

				// remove the wall between them
				ECardinalDirection AtoB = GetWallDirectionToRemove(Dir);
				ECardinalDirection BtoA = GetOppositeDirection(AtoB);

				CurrentTile->HideWall(AtoB, true);
				Neighbor->HideWall(BtoA, true);
			}
		}
	}
}


FIntVector ADungeonManager::RollForDirection(ADungeonPieceActor* SelectedDungeonPiece,TArray<int>& NumbersToIgnore, const TMap<FIntVector,ADungeonPieceActor*>& OccupiedLocations)
{
	int RolledNum = 0;
	FIntVector RolledDirection;
	FIntVector FinalDirection;
	//Keep doing this until we have found a valid spot.
	do {

		if(NumbersToIgnore.Num() > 3)
		{
			return FIntVector(-99);
		}

		//we roll for a direction
		RolledNum = FMath::RandRange(0, 3);
		//if its in the list of already rolled. Restart
		if(NumbersToIgnore.Contains(RolledNum))
		{
			continue;
		}
	//if its not in the list of already rolled, add it.
	NumbersToIgnore.Add(RolledNum);

	switch (RolledNum)
	{
		//straight
	case 0:
		RolledDirection = {1,0,0};
		 break;
	case 1:
		//left
		RolledDirection = {0,-1,0};
		 break;
	case 2:
		//right
		RolledDirection = { 0,1,0 };
		 break;
	case 3:
		//back
		RolledDirection = { -1,0,0 };
		break;

	default:
		RolledDirection = { 1,0,0 };
		break;
	}


		FinalDirection = FIntVector(SelectedDungeonPiece->GetActorLocation()) + RolledDirection * TileSize;
	} while (OccupiedLocations.Contains(FinalDirection));


	return FinalDirection;
}

ADungeonPieceActor* ADungeonManager::GetRandomPiece()
{
	int RolledNum = FMath::RandRange(0, AvailableTiles.Num() - 1);

	if(AvailableTiles.IsValidIndex(RolledNum))
	{
		return AvailableTiles[RolledNum];
	}

	return nullptr;
}

bool ADungeonManager::AreTilesNeighbors(const FIntVector& TileA, const FIntVector& TileB)
{
	FIntVector Delta = TileB - TileA;
	Delta /= TileSize;

	if (Delta == FIntVector(1,0,0)) return true;
	if (Delta == FIntVector(-1, 0, 0)) return true;
	if (Delta == FIntVector(0, 1, 0)) return true;
	if (Delta == FIntVector(0, -1, 0)) return true;

	return false;
}



void ADungeonManager::DFS(ADungeonPieceActor* CurrentTile, ADungeonPieceActor* GoalTile,
	TArray<ADungeonPieceActor*>& Path, TSet<ADungeonPieceActor*>& Visited, bool& bSuccess)
{
	if(bSuccess || !CurrentTile)
	{
		return;
	}
	Visited.Add(CurrentTile);
	Path.Add(CurrentTile);
	if(CurrentTile == GoalTile)
	{
		bSuccess = true;
		return;
	}
	

	TArray<ADungeonPieceActor*>Neighbors = GetUnvisitedNeighbors(CurrentTile, Visited);
	ShuffleArray(Neighbors);

	for(ADungeonPieceActor*Neighbor : Neighbors)
	{
		if (bSuccess) break;

		DFS(Neighbor, GoalTile,Path ,Visited, bSuccess);
	}

	if(!bSuccess)
	{
		Path.Remove(CurrentTile);
	}

}

void ADungeonManager::PlaceTeleporter()
{
	if (!Goal) return;
	for (int i = 0; i < Teleporters.Num(); ++i) {
		ADungeonPieceActor* SelectedPiece = nullptr;
		FIntVector SpawnLocation;
		if (i == 0) {
			 SpawnLocation = FIntVector(Goal->GetActorLocation()) + ((FIntVector(0, 0, 1) * FloorTileZOffset)*Goal->floors);
		}
		else
		{
			 SelectedPiece = CurrentDungeonPieces[FMath::RandRange(1, CurrentDungeonPieces.Num() - 1)];
			SpawnLocation = FIntVector(SelectedPiece->GetActorLocation()) + ((FIntVector(0, 0, 1) * FloorTileZOffset) * SelectedPiece->floors);


		}
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		if (AOrbTeleporter* SpawnedTeleporter = GetWorld()->SpawnActor<AOrbTeleporter>(Teleporters[i], FVector(SpawnLocation), FRotator::ZeroRotator, params)) {

			if (SelectedPiece)
			{
				SelectedPiece->SpawnedActor = SpawnedTeleporter;
			}else if (Goal && i == 0)
			{
				Goal->SpawnedActor = SpawnedTeleporter;
			}
		}
	}
}

TArray<ADungeonPieceActor*> ADungeonManager::GetUnvisitedNeighbors(ADungeonPieceActor* CurrentTile,
	TSet<ADungeonPieceActor*>& Visited)
{
	TArray<ADungeonPieceActor*> ValidNeighbors;
	const TArray<FIntVector> Directions = { {1,0,0}, {-1,0,0},{0,1,0},{0,-1,0}};

	for(const FIntVector& Direction : Directions)
	{
		FIntVector FinalDirection = FIntVector(CurrentTile->GetActorLocation()) + (Direction*TileSize);

		if(ADungeonPieceActor**NeighboringTile = TileGrid.Find(FinalDirection))
		{
			if (*NeighboringTile) {
				if (!Visited.Contains(*NeighboringTile))
				{
					ValidNeighbors.Add(*NeighboringTile);
				}
			}
		}

	}

	return ValidNeighbors;
}


void ADungeonManager::PathRemoveWalls(const TArray<ADungeonPieceActor*>& Path)
{
	for(int i = 0; i < Path.Num(); ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("PathRemoveWalls outer loop"));

 		ADungeonPieceActor* TileA = Path[i];
		for(int j = 0; j < Path.Num(); ++j)
		{
			UE_LOG(LogTemp, Warning, TEXT("PathRemoveWalls inner loop"));

			if (i == j)
			{
				continue;
			}

			ADungeonPieceActor* TileB = Path[j];

			if(AreTilesNeighbors(FIntVector(TileA->GetActorLocation()),FIntVector(TileB->GetActorLocation())))
			{
				FIntVector Delta = FIntVector(TileB->GetActorLocation()) - FIntVector(TileA->GetActorLocation());

				Delta /= TileSize;

				ECardinalDirection TileADirection = GetWallDirectionToRemove(Delta);

				ECardinalDirection TileBDirection = GetOppositeDirection(TileADirection);

				TileA->HideWall(TileADirection, true);
				TileB->HideWall(TileBDirection, true);

				if(TileA && TileB)
				{
					FVector Start = TileA->GetActorLocation() + FVector(0, 0, 50.f);
					FVector End = TileB->GetActorLocation() + FVector(0, 0, 50.f);

					DrawDebugLine(GetWorld(), Start, End, FColor::Red,true,30.f,0.f,5.f);
				}
			}
		}
	}
}

ECardinalDirection ADungeonManager::GetWallDirectionToRemove(FIntVector InputDirection)
{
	TMap<FIntVector, ECardinalDirection> Directions = { {FIntVector(1,0,0),ECardinalDirection::North},{FIntVector(-1,0,0),ECardinalDirection::South},{FIntVector(0,1,0),ECardinalDirection::East},{FIntVector(0,-1,0),ECardinalDirection::West}};

	if(ECardinalDirection* ReturnedDirection = Directions.Find(InputDirection))
	{
		return *ReturnedDirection;
	}

	return ECardinalDirection::North;
}

ECardinalDirection ADungeonManager::GetOppositeDirection(const ECardinalDirection& CardinalDirection)
{
	switch (CardinalDirection)
	{
	case ECardinalDirection::North:
		return ECardinalDirection::South;
	case ECardinalDirection::South:
		return ECardinalDirection::North;
	case ECardinalDirection::East:
		return ECardinalDirection::West;
	case ECardinalDirection::West:
		return ECardinalDirection::East;

		default:
			return ECardinalDirection::North;
	}

}

void ADungeonManager::ShuffleArray(TArray<ADungeonPieceActor*>& ArrayToShuffle)
{

	for(int i = 0; i < ArrayToShuffle.Num(); ++i)
	{
		int RandomLocation = FMath::RandRange(0, ArrayToShuffle.Num() - 1);

		if (i != RandomLocation) {
			//Array[0]
			ADungeonPieceActor* PieceA = ArrayToShuffle[i];
			//Array[x]
			ADungeonPieceActor* Temp = ArrayToShuffle[RandomLocation];
			//Array[x] = Arr[0]
			ArrayToShuffle[RandomLocation] = PieceA;
			//Array[i] = temp
			ArrayToShuffle[i] = Temp;
		}
	}

}

bool ADungeonManager::DoesHaveFreeNeighbors(ADungeonPieceActor* CurrentTile)
{
	const TArray<FIntVector> Directions = { {1,0,0}, {-1,0,0},{0,1,0},{0,-1,0} };

	for (const FIntVector& Direction : Directions)
	{
		FIntVector FinalDirection = FIntVector(CurrentTile->GetActorLocation()) + (Direction * TileSize);

		if (!TileGrid.Contains(FinalDirection))
		{
			return true;
		}

	}
	return false;
}

int ADungeonManager::OpenNeighborsCount(ADungeonPieceActor* Piece)
{
	const TArray<FIntVector> Directions = { {1,0,0}, {-1,0,0},{0,1,0},{0,-1,0} };
	int32 Count = 0;
	for (const FIntVector& Direction : Directions)
	{
		FIntVector FinalDirection = FIntVector(Piece->GetActorLocation()) + (Direction * TileSize);

		if (TileGrid.Contains(FinalDirection))
		{
			++Count;
		}

	}

	return Count;
}

void ADungeonManager::SeparateRooms()
{
	TSet<ADungeonPieceActor*> Visited;

	for (ADungeonPieceActor* Tile : CurrentDungeonPieces)
	{
		if (Visited.Contains(Tile)) continue;

		TArray<ADungeonPieceActor*> Room;
		FloodFillRoom(Tile, Room, Visited);

		// If the room is too small or narrow, it's likely a hallway
		if (IsLikelyHallway(Room))
			continue;

		AllRooms.Add(Room);
	}
}

bool ADungeonManager::IsLikelyHallway(const TArray<ADungeonPieceActor*>& Room)
{

	int TotalNeighbors = 0;
	for (ADungeonPieceActor* Tile : Room)
	{
		TotalNeighbors += OpenNeighborsCount(Tile);
	}

	float AvgNeighbors = (float)TotalNeighbors / Room.Num();

	// Rooms tend to have 3-4 neighbors. Hallways tend to be 1-2.
	return AvgNeighbors <= 2.0f;
}

void ADungeonManager::FloodFillRoom(ADungeonPieceActor* StartTile, TArray<ADungeonPieceActor*>& Room,
	TSet<ADungeonPieceActor*>& Visited)
{
	TArray<ADungeonPieceActor*> Stack;
	Stack.Add(StartTile);
	Visited.Add(StartTile);

	while(Stack.Num() > 0)
	{
		ADungeonPieceActor* CurrentTile = Stack.Pop();
		Room.Add(CurrentTile);

		for(ADungeonPieceActor*Neighbor : GetUnvisitedNeighbors(CurrentTile,Visited))
		{
			if (OpenNeighborsCount(Neighbor) > 1 && OpenNeighborsCount(CurrentTile) > 2)
			{
				Visited.Add(Neighbor);
				Room.Add(Neighbor);
				Stack.Add(Neighbor);
			}

		}
	}
}

void ADungeonManager::MarkDeadendTiles(TArray<ADungeonPieceActor*>& A)
{

	//directional map
	TMap<FIntVector, ECardinalDirection> DirectionMap = {
		{FIntVector(1, 0, 0), ECardinalDirection::North},
		{FIntVector(-1, 0, 0), ECardinalDirection::South},
		{FIntVector(0, 1, 0), ECardinalDirection::East},
		{FIntVector(0, -1, 0), ECardinalDirection::West}
	};

	//loop through all the tiles
	for (ADungeonPieceActor* Tile : CurrentDungeonPieces)
	{
		if(Tile == CurrentDungeonPieces[0])
		{
			continue;
		}
		//current tile neighboring directions
		TArray<ECardinalDirection> NeighborDirections;

		//loop through the directional map.
		for (const auto& Pair : DirectionMap)
		{
			//check the tile neighbor location
			FIntVector NeighborLoc = FIntVector(Tile->GetActorLocation()) + Pair.Key * TileSize;
			//if the tile grid contains a tile at the neighboring location
			if (TileGrid.Contains(NeighborLoc))
			{
				//add it
				NeighborDirections.Add(Pair.Value);
			}
		}
		//check if there are 2 directions
		if (NeighborDirections.Num() == 2)
		{
			// Corner if the directions are adjacent (not opposite)
			if ((NeighborDirections.Contains(ECardinalDirection::North) && NeighborDirections.Contains(ECardinalDirection::East)) ||
				(NeighborDirections.Contains(ECardinalDirection::East) && NeighborDirections.Contains(ECardinalDirection::South)) ||
				(NeighborDirections.Contains(ECardinalDirection::South) && NeighborDirections.Contains(ECardinalDirection::West)) ||
				(NeighborDirections.Contains(ECardinalDirection::West) && NeighborDirections.Contains(ECardinalDirection::North)))
			{
				// This tile is in a corner pocket!
				DeadEndTiles.Add(Tile);
			}
		}
	}

}

void ADungeonManager::Decorate()
{

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//directional map
	TMap<FIntVector, ECardinalDirection> DirectionMap = {
		{FIntVector(1, 0, 0), ECardinalDirection::North},
		{FIntVector(-1, 0, 0), ECardinalDirection::South},
		{FIntVector(0, 1, 0), ECardinalDirection::East},
		{FIntVector(0, -1, 0), ECardinalDirection::West}
	};
	for(int i = 0; i < CurrentDungeonPieces.Num(); ++i)
	{
		if(i % 2 != 0)
		{
			continue;
		}

		for(const auto& pair : DirectionMap)
		{
			FIntVector FinalDirection = FIntVector(CurrentDungeonPieces[i]->GetActorLocation()) + (pair.Key * TileSize);

			if(!TileGrid.Contains(FinalDirection))
			{

				TSubclassOf<AActor> RolledActor = RoleActorToSpawn(CurrentDungeonPieces[i]->DecorativeWallActorsMap);

				switch (pair.Value)
				{
				case ECardinalDirection::North:
					if(UArrowComponent* ArrowComp = Cast<UArrowComponent>(CurrentDungeonPieces[i]->NorthSocketRef.GetComponent(CurrentDungeonPieces[i])))
					{
						AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(RolledActor, ArrowComp->GetComponentTransform(), params);
					}
					break;
				case ECardinalDirection::South:
					if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(CurrentDungeonPieces[i]->SouthSocketRef.GetComponent(CurrentDungeonPieces[i])))
					{
						AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(RolledActor, ArrowComp->GetComponentTransform(), params);
					}
					break;
				case ECardinalDirection::East:
					if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(CurrentDungeonPieces[i]->EastSocketRef.GetComponent(CurrentDungeonPieces[i])))
					{
						AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(RolledActor, ArrowComp->GetComponentTransform(), params);
					}
					break;
				case ECardinalDirection::West:
					if (UArrowComponent* ArrowComp = Cast<UArrowComponent>(CurrentDungeonPieces[i]->WestSocketRef.GetComponent(CurrentDungeonPieces[i])))
					{
						AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(RolledActor, ArrowComp->GetComponentTransform(), params);
					}
					break;
				}
			}
		}

	}
}

TSubclassOf<AActor> ADungeonManager::RoleActorToSpawn(const TMap<TSubclassOf<AActor>, float>& Map)
{
	float total = 0.f;
	for (const auto& pair : Map)
	{
		total += pair.Value;
	}

	float RandomRoll = FMath::FRandRange(0, total);

	for (const auto& pair : Map)
	{
		RandomRoll-= pair.Value;

		if(RandomRoll <= 0)
		{
			return pair.Key;
		}
	}

	return nullptr;
}

void ADungeonManager::SetFloorType(ADungeonPieceActor* DungeonPieceActor,EPieceType NewType)
{
	DungeonPieceActor->PieceType = NewType;
	//DungeonPieceActor->AssignMaterial();

}

void ADungeonManager::SetRoomTileType()
{
	for (TArray<ADungeonPieceActor*>& Room : AllRooms)
	{
		float Roll = FMath::FRandRange(0.f, 1.f);

		for (int i = 0; i < Room.Num(); ++i) {
			if (Roll >= .75)
			{
				Room[i]->PieceType = EPieceType::Crypt;
			}
			else if (Roll >= 0.5)
			{
				Room[i]->PieceType = EPieceType::Cobble;
			}
			else if (Roll >= 0.25)
			{
				Room[i]->PieceType = EPieceType::House;
			}
			else
			{
				Room[i]->PieceType = EPieceType::Grass;
			}

			SetWallMaterials(Room[i]);
		}
	}
}

void ADungeonManager::SetRoomTheme()
{


	TMap<const FDungeonDecorationStruct*,FDungeonDecoRunTime> BudgetMap;

	for (const auto& decoration : Decorations) {
		FDungeonDecoRunTime RunTimeInfo;
			int budget = FMath::RoundToInt(decoration.Density * CurrentDungeonPieces.Num());
			RunTimeInfo.CalculatedBudget = budget;
			BudgetMap.Add(&decoration,RunTimeInfo);
	}

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ShuffleArray(CurrentDungeonPieces);
	if (player) {
		for (ADungeonPieceActor* Tile : CurrentDungeonPieces)
		{
			float Distance = (Tile->GetActorLocation() - player->GetActorLocation()).Size();
			if (Tile->SpawnedActor || OpenNeighborsCount(Tile) == 2 || Distance <= 1000.f)
			{
				continue;
			}

			FVector Location = Tile->GetActorLocation() + ((FVector::UpVector * FloorTileZOffset) * Tile->floors);
			FRotator Rotation = Tile->GetWallTransform().Rotator();
			for (auto& pair : BudgetMap)
			{
				if (pair.Value.CalculatedBudget <= 0 || Tile->PieceType != pair.Key->PieceType)
				{
					continue;
				}

				if (!pair.Value.LastPlaced.IsEmpty())
				{
					float TileDistance = (pair.Value.LastPlaced.Last() - Tile->GetActorLocation()).Size();
					if(TileDistance <= 1000.f)
					{
						continue;
					}
				}

				if (AEnvironmentalItem* EnvironmentalItem = GetWorld()->SpawnActor<AEnvironmentalItem>(pair.Key->DecorationPiece, Location, Rotation, params)) {

					if(OpenNeighborsCount(Tile) <= 3 && pair.Key->Type == EDecorationType::Wallside)
					{
						FVector NewLocation = Tile->GetWallTransform().GetLocation() + (Tile->GetWallTransform().GetUnitAxis(EAxis::X) * 20.f);
						NewLocation.Z = (Tile->GetActorLocation() + ((FVector::UpVector * FloorTileZOffset) * Tile->floors)).Z;
						EnvironmentalItem->SetActorLocation(NewLocation);
					}
					pair.Value.LastPlaced.Add(Tile->GetActorLocation());
					Tile->SpawnedActor = EnvironmentalItem;
					--pair.Value.CalculatedBudget;
					break;
				}
			}
		}
	}


}

void ADungeonManager::SetTrees(const TArray<ADungeonPieceActor*>& Room)
{


	int CurrentTrees = 0;
	for (int i = 0; i < Room.Num(); ++i)
	{
		if (Room[i]->PieceType != EPieceType::Grass) return;

		if (ADungeonPieceActor* Tile = Room[i]; Tile != CurrentDungeonPieces[0]) {

			int maxTrees = FMath::RandRange(Room.Num() / 3, Room.Num());

			if (!Tile->SpawnedActor && CurrentTrees < maxTrees && FMath::RandBool())
			{

				FVector Location = Tile->GetActorLocation() + (FVector::UpVector * 10.f);
				FRotator Rotation = FRotator(0, FMath::RandRange(-180, 180), 0);
				if (TSubclassOf<AActor> SelectedClass = RoleActorToSpawn(TreeClass)) {
					Tile->SpawnedActor = GetWorld()->SpawnActor<AActor>(SelectedClass, Location, Rotation);
					++CurrentTrees;
				}
			}
		}
	}
}

void ADungeonManager::SetColumns(ADungeonPieceActor* SelectedTile)
{
	if (OpenNeighborsCount(SelectedTile) != 4) return;

	TArray<FIntVector> Directions = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0} };

	for(const FIntVector& Direction : Directions)
	{

		FIntVector FinalDirection = FIntVector(SelectedTile->GetActorLocation()) + (Direction * TileSize);


		if (TileGrid.Contains(FinalDirection)) {

			FinalDirection.Z += 50.f;
			if(ADungeonPieceActor**NeighborPtr = TileGrid.Find(FinalDirection))
			{

				(*NeighborPtr)->SpawnedActor = GetWorld()->SpawnActor<AActor>(ColumnToSpawn, FVector(FinalDirection), FRotator::ZeroRotator);
			}

			//DrawDebugSphere(GetWorld(), FVector(FinalDirection), 100.f, 30, FColor::Green,true);
		}
	}

}

void ADungeonManager::PlaceFakeWalls()
{
	for(ADungeonPieceActor*Tile : DeadEndTiles)
	{

		TArray<FIntVector> Directions = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0} };

		float Distance = (CurrentDungeonPieces[0]->GetActorLocation() - Tile->GetActorLocation()).Size();

		if(Distance <= 2000.f)
		{
			continue;
		}

		for (const FIntVector& Direction : Directions)
		{

			FIntVector FinalDirection = FIntVector(Tile->GetActorLocation()) + (Direction * TileSize);


			if (TileGrid.Contains(FinalDirection) && CurrentFakeWalls < MaxFakeWalls) {

				ECardinalDirection WallDirection = GetWallDirectionToRemove(Direction);
				UStaticMeshComponent* Wall = nullptr;
				switch(WallDirection)
				{
				case ECardinalDirection::North:
					Wall = Tile->Walls[0];
					break;
				case ECardinalDirection::South:
					Wall = Tile->Walls[1];
					break;
				case ECardinalDirection::East:
					Wall = Tile->Walls[2];

					break;
				case ECardinalDirection::West:
					Wall = Tile->Walls[3];

					break;

					default:					
						Wall = Tile->Walls[0];
						break;
				}
				
				GetWorld()->SpawnActor<AActor>(FakeWallToSpawn, Wall->GetComponentTransform());

				//DrawDebugSphere(GetWorld(), FVector(FinalDirection), 100.f, 30, FColor::Green, true);
			}
		}

		++CurrentFakeWalls;
	}
}

void ADungeonManager::PlaceDoorway()
{

	TArray<FIntVector> Directions = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0} };
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for(TArray<ADungeonPieceActor*>& Room : AllRooms)
	{
		for (ADungeonPieceActor* Tile : Room) {

			for (const FIntVector& Direction : Directions)
			{
				FIntVector FinalDirection = FIntVector(Tile->GetActorLocation()) + (Direction * TileSize);
				if (ADungeonPieceActor** Neighbor = TileGrid.Find(FinalDirection))
				{
					FVector TilePos = Tile->GetActorLocation();
					FVector NeighborPos = (*Neighbor)->GetActorLocation();

			
						if (!Room.Contains(*Neighbor) &&
						(TilePos.X < NeighborPos.X ||
							(TilePos.X == NeighborPos.X && TilePos.Y <= NeighborPos.Y)))
					{
						ECardinalDirection CardinalDirection = GetWallDirectionToRemove(Direction);
						TArray<ECardinalDirection> AdjSides = AdjacentSides(CardinalDirection);
						UStaticMeshComponent* Wall = nullptr;

						switch (CardinalDirection) 
						{
						case ECardinalDirection::North:
							Wall = Tile->Walls[0];
							break;
						case ECardinalDirection::South:
							Wall = Tile->Walls[1];
							break;
						case ECardinalDirection::East:
							Wall = Tile->Walls[2];
							break;
						case ECardinalDirection::West:
							Wall = Tile->Walls[3];
							break;
						default:
							Wall = Tile->Walls[0];
							break;
						}

							
						for (ECardinalDirection Dir : AdjSides)
						{
							FIntVector Offset = Dir == ECardinalDirection::North ? FIntVector(1, 0, 0) :
								Dir == ECardinalDirection::South ? FIntVector(-1, 0, 0) :
								Dir == ECardinalDirection::East ? FIntVector(0, 1, 0) :
								FIntVector(0, -1, 0);

							FIntVector NeighPos = FIntVector(Tile->GetActorLocation()) + Offset * TileSize;

							if (TileGrid.Contains(NeighPos))           // edge of the room/hallway
							{
								if (UStaticMeshComponent** SideWall = Tile->HiddenWalls.Find(Dir))
								{
									Tile->ActivateWall(*SideWall);
								}
							}
						}

						if (Wall) {
							//DrawDebugSphere(GetWorld(), Wall->GetComponentLocation(), 100.f, 12, FColor::Magenta, true);
							GetWorld()->SpawnActor<AActor>(DoorwayToSpawn, Wall->GetComponentTransform(), params);
						}
					}

				}
			}
		}
	}
}

TSubclassOf<AEnvironmentalItem> ADungeonManager::RollForEnvironmentalItem(
	TMap<float, TSubclassOf<AEnvironmentalItem>>& Map)
{
	float total = 0.f;
	for (const auto& pair : Map)
	{
		total += pair.Key;
	}

	float RandomRoll = FMath::FRandRange(0, total);

	for (const auto& pair : Map)
	{
		RandomRoll -= pair.Key;

		if (RandomRoll <= 0)
		{
			return pair.Value;
		}
	}

	return nullptr;
}

TArray<ADungeonPieceActor*> ADungeonManager::FreeTilesArray()
{

	TArray<ADungeonPieceActor*> FreeTiles;


	for (int i = 0; i < CurrentDungeonPieces.Num(); ++i)
	{
		if (ADungeonPieceActor* Tile = CurrentDungeonPieces[i]) {

			float Distance = (CurrentDungeonPieces[0]->GetActorLocation() - Tile->GetActorLocation()).Size();

			if (!Tile->SpawnedActor && Distance > 2000.f)
			{
				FreeTiles.Add(Tile);
			}
		}
	}

	return FreeTiles;
}

void ADungeonManager::SetPlayer()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (APlayerCharacter* Player = MGI->localPlayer)
		{

			Player->SpawnLocation = CurrentDungeonPieces[0]->GetActorLocation() + (FVector::UpVector * 100.f);
			Player->SetActorLocation(Player->SpawnLocation);
			player = Player;
		}
	}
}

void ADungeonManager::SetRemainingEnemies()
{
	TArray<ADungeonPieceActor*> UnusedTiles = FreeTilesArray();

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ShuffleArray(UnusedTiles);

	for (int i = 0; i < UnusedTiles.Num() && CurrentEnemies < MaxEnemiesToSpawn; ++i) {
		if (ADungeonPieceActor* RandomTile = UnusedTiles[i]; !RandomTile->SpawnedActor)
		{
			FVector SpawnLocation = RandomTile->GetActorLocation();

			SpawnLocation.Z += 100.f;
			TSubclassOf<AActor> SelectedActor = RoleActorToSpawn(RandomTile->SpawningEnemies);
			if (AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SelectedActor, SpawnLocation, FRotator::ZeroRotator, params))
			{
				RandomTile->SpawnedActor = SpawnedActor;
				if (ARoamingSpawnPoint* RoamingSpawnPoint = Cast<ARoamingSpawnPoint>(SpawnedActor))
				{
					RoamingSpawnPoint->IsProcedural = true;
				}
				++CurrentEnemies;
			}

		}
	}
}

void ADungeonManager::SetEnemiesInRooms()
{


	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (TArray<ADungeonPieceActor*>& Room : AllRooms)
	{
		float size = Room.Num() * (TileSize * TileSize);

		if (CurrentEnemies < MaxEnemiesToSpawn)
		{
			int EnemiesToSpawn = 0;

			if (size >= 2500000)
			{
				EnemiesToSpawn = 3;
			}
			else if (size >= 1250000)
			{
				EnemiesToSpawn = 2;
			}
			else
			{
				EnemiesToSpawn = 1;
			}


			ShuffleArray(Room);

			for (int i = 0; i < Room.Num() && CurrentEnemies < EnemiesToSpawn; ++i)
			{
				if (ADungeonPieceActor* SelectedTile = Room[i])
				{
					if (SelectedTile->SpawnedActor)
					{
						continue;
					}
					float Distance = (CurrentDungeonPieces[0]->GetActorLocation() - SelectedTile->GetActorLocation()).Size();

					if (Distance <= 2000.f)
					{
						continue;
					}

					TSubclassOf<AActor> SelectedActor = RoleActorToSpawn(SelectedTile->SpawningEnemies);
					UE_LOG(LogTemp, Warning, TEXT("Spawning Enemy on %s,%f"), *SelectedTile->GetName(), size);
					//DrawDebugSphere(GetWorld(), SelectedTile->GetActorLocation(), 100.f, 30, FColor::Red, true);
					FVector Location = SelectedTile->GetActorLocation() + (FVector::UpVector * 100.f);
					if (AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SelectedActor, Location, FRotator::ZeroRotator, params))
					{
						SelectedTile->SpawnedActor = SpawnedActor;
						if (ARoamingSpawnPoint* RoamingSpawnPoint = Cast<ARoamingSpawnPoint>(SpawnedActor))
						{
							RoamingSpawnPoint->IsProcedural = true;
						}
					}
					++CurrentEnemies;
				}

			}

		}

	}
}

void ADungeonManager::SetDeadEndItems()
{

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (DeadEndTiles.Num() > 1)
	{
		for (int i = 0; i < DeadEndTiles.Num(); ++i) {
			if (ADungeonPieceActor* SelectedTile = DeadEndTiles[i])
			{
				TSubclassOf<AActor> SelectedObject = RoleActorToSpawn(SelectedTile->CornerObjectsMap);
				FVector FinalLocation = SelectedTile->GetActorLocation() + ((FVector::UpVector *FloorTileZOffset)*SelectedTile->floors);
				FRotator Rotation = FRotator::ZeroRotator;
				if (ADungeonPieceActor* Neighbor = GetNeighboringTile(SelectedTile)) {

					FVector DirectionAwayFromWall = (Neighbor->GetActorLocation() - SelectedTile->GetActorLocation()).GetSafeNormal();
					Rotation = DirectionAwayFromWall.Rotation();

				}
				if (AActor* SpawnedObject = GetWorld()->SpawnActor<AActor>(SelectedObject, FinalLocation, Rotation, params)) {
					SelectedTile->SpawnedActor = SpawnedObject;
				}
			}
		}
	}
}

TArray<ECardinalDirection> ADungeonManager::AdjacentSides(ECardinalDirection CurrentDirection)
{
	TArray<ECardinalDirection> Sides;
	switch (CurrentDirection)
	{
	case ECardinalDirection::North:
	case ECardinalDirection::South:
		Sides = { ECardinalDirection::East, ECardinalDirection::West };
		break;
	case ECardinalDirection::East:
	case ECardinalDirection::West:
		Sides = { ECardinalDirection::North, ECardinalDirection::South };
		break;
	}

	return Sides;
}

void ADungeonManager::PlaceRandomItems(TArray<ADungeonPieceActor*>& Room)
{
	ShuffleArray(Room);
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for(ADungeonPieceActor*Tile : Room)
	{

		float Roll = FMath::FRandRange(0.0, 1.f);
		
		if(!Tile->SpawnedActor && Roll >= 0.9)
		{
			TSubclassOf<AActor> SelectedActor = RoleActorToSpawn(RandomItemsMap);

			if (AActor* SpawnedObject = GetWorld()->SpawnActor<AActor>(SelectedActor, Tile->GetActorTransform(), params)) {
				Tile->SpawnedActor = SpawnedObject;
			}
		}
	}
}

void ADungeonManager::AssignGroundMaterial(EPieceType Type,AEnvironmentalItem* FloorTile)
{
	if (!FloorTile || !FloorTile->Mesh) return;

		if (UMaterialInstance* Material = *TileMaterialMap.Find(Type))
		{
				FloorTile->Mesh->SetMaterial(0, Material);
		}
	
}

void ADungeonManager::PlaceFloorTiles()
{
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for(ADungeonPieceActor*Tile : CurrentDungeonPieces)
	{

	//	float RolledNumber = FMath::FRandRange(0, 1.f);

		for(int i = 1; i < Tile->TileHeight; ++i)
		{
			if (UStaticMeshComponent* FloorMesh = Cast<UStaticMeshComponent>(Tile->FloorRef.GetComponent(Tile))) {
				FVector Location = FloorMesh->GetComponentLocation() + FVector(0,0,(i * FloorTileZOffset));
				FRotator Rotation = FloorMesh->GetComponentRotation();
				if(AEnvironmentalItem* FloorTile = GetWorld()->SpawnActor<AEnvironmentalItem>(FloorToSpawn, Location, Rotation, params))
				{
					++Tile->floors;
					AssignGroundMaterial(Tile->PieceType, FloorTile);
				}
			}
		}
	}
}

void ADungeonManager::InitScaleEnemies(int RoundCount)
{
	TArray<AActor*> EnemyActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyActors);

	float Scale = FMath::Pow(1.1f, RoundCount - 1); // Grows 10% per round

	for(AActor* ActorEnemy : EnemyActors)
	{
		if(AEnemy*Enemy = Cast<AEnemy>(ActorEnemy))
		{
			Enemy->Health = Enemy->MaxHealth * Scale;
			Enemy->SetStamina(Enemy->MaxStamina * Scale);
			Enemy->EnemyParameters.ElementDamage = Enemy->EnemyParameters.ElementDamage * Scale;
			Enemy->Damage *= Scale;

		}
	}
}

void ADungeonManager::ResetDungeon()
{


	CleanWorld();


	TArray<AActor*> OrbActors;
	//previous orbs need to be destroyed
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbTeleporter::StaticClass(), OrbActors);
	

		for(AActor*CurrentTeleporter : OrbActors)
		{
			if (CurrentTeleporter) {
				CurrentTeleporter->Destroy();
			}
		}


	TArray<AActor*> MusicActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelMusicActor::StaticClass(), MusicActor);
	for (AActor* Music : MusicActor)
	{
		if (MainMusicActor.IsValid() && Music != MainMusicActor)
		{
			Music->Destroy();
		}
	}

	if(MainMusicActor.IsValid())
	{
		if(ALevelMusicActor*LocalMainMusicActor = MainMusicActor.Get())
		{
			LocalMainMusicActor->SetAndPlayMusic();
		}
	}

	TArray<AActor*> LeftOverItems;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AItem::StaticClass(), LeftOverItems);

	for(AActor*ItemActor : LeftOverItems)
	{
		if (!ItemActor) continue;

		if (player)
		{
			if(APlayerCharacterState* PCS = player->GetPlayerCharacterState())
			{
				if(ItemActor)
				{
					if (AItem* Item = Cast<AItem>(ItemActor)) {
						if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(Item->ItemData))
						{
							if(PCS->PlayerGear.Contains(WeaponItemData->GearType))
							{
								continue;
							}
						}
						Item->Destroy();
					}
				}
			}
		}
	}

	if (ADungeonCrawlerGM* DCGM = Cast<ADungeonCrawlerGM>(GetWorld()->GetAuthGameMode()))
	{
		DCGM->IncrementRoundCount(1);
	}

	InitializeWorld();
}

void ADungeonManager::SetWallMaterials(ADungeonPieceActor*Tile)
{
	if (!Tile) return;

	auto SetWallMats = [this](ADungeonPieceActor*Tile)
	{
			for (UStaticMeshComponent* WallMesh : Tile->Walls)
			{
				if (WallMesh)
				{
					if (UMaterialInstance** FoundMaterial = TileMaterialMap.Find(Tile->PieceType))
					{
						WallMesh->SetMaterial(0, *FoundMaterial);
					}
				}
			}
	};

		//both cobble and grass can be ignored because the default wall type is cobble.
		if(Tile->PieceType == EPieceType::Crypt || Tile->PieceType == EPieceType::House)
		{
			SetWallMats(Tile);
		}
	
}

bool ADungeonManager::bIsTileNextToHallway(ADungeonPieceActor* Tile)
{
	TArray<FIntVector> Directions = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0} };

	for (const auto& Direction : Directions)
	{
		FIntVector Location = FIntVector(Tile->GetActorLocation()) + (Direction * TileSize);

		if(ADungeonPieceActor**Neighbor = TileGrid.Find(Location))
		{
			if(OpenNeighborsCount(*Neighbor) <= 2)
			{
				return true;
			}
		}
	}

	return false;
}

ADungeonPieceActor* ADungeonManager::GetNeighboringTile(ADungeonPieceActor*Tile)
{
	TArray<FIntVector> Directions = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0} };
	for (const auto& Direction : Directions)
	{
		FIntVector Location = FIntVector(Tile->GetActorLocation()) + (Direction * TileSize);

		if (ADungeonPieceActor** Neighbor = TileGrid.Find(Location))
		{
			return *Neighbor;
		}
	}

	return nullptr;
	
}


void ADungeonManager::PlaceQuest()
{
	float ShouldQuestAppear = FMath::FRandRange(0, 1.f);

	if (ShouldQuestAppear >= 0.75) return;

	TArray<ADungeonPieceActor*> EligiblePieces;
	for(int i = 5; i < CurrentDungeonPieces.Num(); ++i)
	{
		if(CurrentDungeonPieces.IsValidIndex(i))
		{
			if (CurrentDungeonPieces[i]->SpawnedActor)
			{
				continue;
			}
				EligiblePieces.Add(CurrentDungeonPieces[i]);
		}
	}

	int RandomNum = FMath::RandRange(0, EligiblePieces.Num() - 1);

	if(EligiblePieces.IsValidIndex(RandomNum))
	{
		FVector SpawnLocation = FVector(FIntVector(EligiblePieces[RandomNum]->GetActorLocation()) + (FIntVector(0, 0, 1) * FloorTileZOffset * EligiblePieces[RandomNum]->floors));
		FVector NeighborLocation = (GetNeighboringTile(EligiblePieces[RandomNum])->GetActorLocation() - SpawnLocation).GetSafeNormal();
		GetWorld()->SpawnActor<ANPC_QuestGiver>(QuestGiver, SpawnLocation,NeighborLocation.Rotation());
	}
}

void ADungeonManager::CleanWorld()
{
	for (ADungeonPieceActor* Tile : CurrentDungeonPieces)
	{
		if (Tile) {
			Tile->Destroy();
		}
	}

	TArray<AActor*> EnvrionmentalActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentalItem::StaticClass(), EnvrionmentalActors);

	for (AActor* Actor : EnvrionmentalActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	//respawn points need to be destroyed
	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), SpawnPoints);

	for (AActor* SpawnPoint : SpawnPoints)
	{
		if (SpawnPoint)
		{
			SpawnPoint->Destroy();
		}
	}

	TArray<AActor*> Treasure;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATreasureChest::StaticClass(), Treasure);

	for (AActor* TreasureActor : Treasure)
	{
		if (TreasureActor)
		{
			TreasureActor->Destroy();
		}
	}

	TArray<AActor*> OrbActors;
	//previous orbs need to be destroyed
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOrbTeleporter::StaticClass(), OrbActors);


	for (AActor* CurrentTeleporter : OrbActors)
	{
		if (CurrentTeleporter) {
			CurrentTeleporter->Destroy();
		}
	}

	TArray<AActor*> InteractableActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInteractableActor::StaticClass(), InteractableActors);

	for (AActor* InteraActor : InteractableActors)
	{
		if (InteraActor)
		{
			InteraActor->Destroy();
		}
	}

	TArray<AActor*> Enemies;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), Enemies);
	for (AActor* Enemy : Enemies)
	{
		Enemy->Destroy();
	}


	TArray<AActor*> NPCS;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPC::StaticClass(), NPCS);

	for(AActor*NPC : NPCS)
	{
		NPC->Destroy();
	}


	CurrentDungeonPieces.Empty();
	AvailableTiles.Empty();
	TileGrid.Empty();
	AllRooms.Empty();
	DeadEndTiles.Empty();
	CurrentEnemies = 0;
	CurrentFakeWalls = 0;

}

void ADungeonManager::SetResetTeleporter(AOrbTeleporter* ResetTeleporter)
{
	Teleporter = ResetTeleporter;
}

void ADungeonManager::ReactivateTeleporter(AOrbTeleporter* TeleporterToReactivate)
{
	if(IsValid(TeleporterToReactivate))
	{
		if(UBoxComponent*BoxComp = TeleporterToReactivate->BoxComponent)
		{
			BoxComp->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		}
	}
}

void ADungeonManager::ToggleActors()
{
	if (!player) return;

	// Enemies
	for (AActor* EnemyActor : CurrentEnemiesArr)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(EnemyActor))
		{
			float Distance = (Enemy->GetActorLocation() - player->GetActorLocation()).Size();
			bool bShouldEnable = Distance <= OptimizeDistance;

			// Always explicitly set
			Enemy->ToggleEnemy(bShouldEnable);
		}
	}

	// Environmental Items
	for (AActor* EnvironmentalItemActor : CurrentEnvironmentalItems)
	{
		if (AEnvironmentalItem* EnvironmentalItem = Cast<AEnvironmentalItem>(EnvironmentalItemActor))
		{
			float Distance = (EnvironmentalItem->GetActorLocation() - player->GetActorLocation()).Size();
			bool bShouldEnable = Distance <= OptimizeDistance;

			EnvironmentalItem->ToggleEnvironmentalItem(bShouldEnable);
		}
	}

	// Dungeon Tiles
	for (AActor* DungeonTile : CurrentDungeonPieces)
	{
		if (ADungeonPieceActor* Tile = Cast<ADungeonPieceActor>(DungeonTile))
		{
			float Distance = (Tile->GetActorLocation() - player->GetActorLocation()).Size();
			bool bShouldEnable = Distance <= OptimizeDistance;

			Tile->ToggleDungeonPiece(bShouldEnable);
		}
	}
}


// Called when the game starts or when spawned
void ADungeonManager::BeginPlay()
{
	Super::BeginPlay();

	
	InitializeWorld();

}

// Called every frame
void ADungeonManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ToggleActors();

}
