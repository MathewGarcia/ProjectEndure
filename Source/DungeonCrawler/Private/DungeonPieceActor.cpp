// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonPieceActor.h"
#include "LiquidActor.h"
#include "CardinalDirections.h"
#include "Components/BoxComponent.h"

static UStaticMeshComponent* GetWall(const TArray<FComponentReference>& Arr,
                                     int32 Floor,  AActor* Owner)
{
    return Arr.IsValidIndex(Floor)
        ? Cast<UStaticMeshComponent>(Arr[Floor].GetComponent(Owner))
        : nullptr;
}

// Sets default values
ADungeonPieceActor::ADungeonPieceActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    NorthSocketRef.ComponentProperty = "NorthSocket";
    SouthSocketRef.ComponentProperty = "SouthSocket";
    WestSocketRef.ComponentProperty = "WestSocket";
    EastSocketRef.ComponentProperty = "EastSocket";
    FloorRef.ComponentProperty = "Floor";
}

void ADungeonPieceActor::HideWall(ECardinalDirection WallToHide, bool bHide)
{
    switch (WallToHide)
    {
    case ECardinalDirection::North:
        if (Walls.IsValidIndex(0)) {
            if (UStaticMeshComponent* NorthWallMesh = Walls[0])
            {
                NorthWallMesh->SetHiddenInGame(bHide);
                NorthWallMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
                HiddenWalls.Add(ECardinalDirection::North,NorthWallMesh);
            }
        }
        break;
    case ECardinalDirection::South:
        if (Walls.IsValidIndex(1)) {
            if (UStaticMeshComponent* SouthWallMesh = Walls[1])
            {
                SouthWallMesh->SetHiddenInGame(bHide);
                SouthWallMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
                HiddenWalls.Add(ECardinalDirection::South,SouthWallMesh);
            }
        }
        break;
    case ECardinalDirection::East:
        if (Walls.IsValidIndex(2)) 
        if(UStaticMeshComponent* EastWallMesh = Walls[2])
        {
            EastWallMesh->SetHiddenInGame(bHide);
            EastWallMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
            HiddenWalls.Add(ECardinalDirection::East,EastWallMesh);
        }
        break;
    case ECardinalDirection::West:
        if (Walls.IsValidIndex(3))
        if(UStaticMeshComponent* WestWallMesh = Walls[3])
        {
            WestWallMesh->SetHiddenInGame(bHide);
            WestWallMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
            HiddenWalls.Add(ECardinalDirection::West,WestWallMesh);
        }
        break;
    default:
        if (Walls.IsValidIndex(0)) 
        if (UStaticMeshComponent* NorthWallMesh = Walls[0])
        {
            NorthWallMesh->SetHiddenInGame(bHide);
            NorthWallMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
            HiddenWalls.Add(ECardinalDirection::North, NorthWallMesh);
        }
        break;
    }
}

void ADungeonPieceActor::AssignMaterial()
{
    if(UMaterialInstance** MaterialPtr = MaterialMap.Find(Liquid))
    {
        if(UMaterialInstance* Material = *MaterialPtr)
        {
            if(UStaticMeshComponent*FloorMesh = Cast<UStaticMeshComponent>(FloorRef.GetComponent(this)))
            {
                FloorMesh->SetMaterial(0, Material);
            }
        }
    }
}

void ADungeonPieceActor::ActivateWall(UStaticMeshComponent* WallMesh)
{
    if (WallMesh) {
        WallMesh->SetHiddenInGame(false);
        WallMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
    }
}

void ADungeonPieceActor::SetFloorType(const int& FloorType)
{
    switch (FloorType)
    {
    case 0: Liquid = ELiquidType::Lava;
        break;
    case 1: Liquid = ELiquidType::Water;
        break;
    case 2:
        Liquid = ELiquidType::Poison;
        break;
    case 3:
        Liquid = ELiquidType::Blood;
        break;
    default:  Liquid = ELiquidType::Lava;
        break;
    }
    AssignMaterial();
}

void ADungeonPieceActor::SetFloorMesh(const int& RolledNum)
{
    FActorSpawnParameters params;
    params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    if (UStaticMeshComponent* Floor = Cast<UStaticMeshComponent>(FloorRef.GetComponent(this))) {
        if (LiquidFloorToSpawn && GetWorld())
        {
            FVector Location = Floor->Bounds.Origin;
            if (ALiquidActor* SpawnedLiquid = GetWorld()->SpawnActor<ALiquidActor>(LiquidFloorToSpawn, Location,FRotator::ZeroRotator))
            {
                switch (RolledNum)
                {
                case 0: Liquid = ELiquidType::Lava;
                    SpawnedLiquid->ElementType = EElementTypes::Fire;
                    break;
                case 1: Liquid = ELiquidType::Water;
                    SpawnedLiquid->ElementType = EElementTypes::Water;
                    break;
                case 2:
                    Liquid = ELiquidType::Poison;
                    SpawnedLiquid->ElementType = EElementTypes::Poison;
                    break;
                case 3:
                    Liquid = ELiquidType::Blood;
                    SpawnedLiquid->ElementType = EElementTypes::Bleed;
                    break;
                default:  Liquid = ELiquidType::Lava;
                    SpawnedLiquid->ElementType = EElementTypes::Fire;
                    break;
                }
            }
        }
    }
}

FTransform ADungeonPieceActor::GetWallTransform()
{
    for(auto*Wall : Walls)
    {
        if(Wall && Wall->IsVisible())
        {
            if (Wall->GetNumChildrenComponents() > 0 && Wall->GetChildComponent(0)) {
                return Wall->GetChildComponent(0)->GetComponentTransform();
            }
            return Wall->GetComponentTransform();
        }
    }
    return FTransform::Identity;
}

void ADungeonPieceActor::ToggleDungeonPiece(bool bNewVal)
{
    bIsEnabled = bNewVal;
    //SetActorEnableCollision(!bNewVal);
    SetActorHiddenInGame(!bNewVal);
}

// Called when the game starts or when spawned
void ADungeonPieceActor::BeginPlay()
{
    Super::BeginPlay();
    if (UStaticMeshComponent* NorthWallMesh = Cast<UStaticMeshComponent>(NorthWall.GetComponent(this)))
    {
        Walls.Add(NorthWallMesh);
    }
    if (UStaticMeshComponent* SouthWallMesh = Cast<UStaticMeshComponent>(SouthWall.GetComponent(this)))
    {
        Walls.Add(SouthWallMesh);
    }
    if (UStaticMeshComponent* EastWallMesh = Cast<UStaticMeshComponent>(EastWall.GetComponent(this)))
    {
        Walls.Add(EastWallMesh);
    }
    if (UStaticMeshComponent* WestWallMesh = Cast<UStaticMeshComponent>(WestWall.GetComponent(this)))
    {
        Walls.Add(WestWallMesh);
    }
}

// Called every frame
void ADungeonPieceActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

