// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvironmentalItem.h"

#include "Item.h"
#include "MainGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnvironmentalItem::AEnvironmentalItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	 Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh Comp"));
	 RootComponent = Mesh;
}


void AEnvironmentalItem::Destroyed()
{

	Super::Destroyed();
}

void AEnvironmentalItem::ToggleEnvironmentalItem(bool bNewVal)
{
	if (bIsEnabled != bNewVal)
	{
		bIsEnabled = bNewVal;
		SetActorHiddenInGame(!bNewVal);

		if (bWasSimulatingPhysics && Mesh)
		{
			Mesh->SetSimulatePhysics(bNewVal);
		}
	}
}


void AEnvironmentalItem::DropItem()
{
	float ShouldDropItem = FMath::FRandRange(0.f, 1.f);

	if(ShouldDropItem <= 0.1)
	{
		if (ItemToDropClass.Num() > 0)
		{
			int32 Random = FMath::RandRange(0, ItemToDropClass.Num() - 1);
			if (ItemToDropClass.IsValidIndex(Random))
			{
				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				if (GetWorld()) {
					GetWorld()->SpawnActor<AItem>(ItemToDropClass[Random], GetActorLocation(), GetActorRotation(), params);
				}
			}
		}
	}

}


float AEnvironmentalItem::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{

	Health -= DamageAmount;

	if (Health <= 0)
	{
		DropItem();

		UWorld* World = GetWorld();
		if (World) {
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
			{
				if (!MGI->EnvironmentalItemBreaks.IsEmpty())
				{
					USoundBase* SoundToPlay = MGI->EnvironmentalItemBreaks[FMath::RandRange(0, MGI->EnvironmentalItemBreaks.Num() - 1)];

					if (SoundToPlay)
					{
						UGameplayStatics::PlaySoundAtLocation(World, SoundToPlay, GetActorLocation());
					}

				}
			}
		}

		Destroy();
	}

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

// Called when the game starts or when spawned
void AEnvironmentalItem::BeginPlay()
{
	Super::BeginPlay();

	if (Mesh) {
		bWasSimulatingPhysics = Mesh->IsSimulatingPhysics();
	} else {
		bWasSimulatingPhysics = false;
	}

}

// Called every frame
void AEnvironmentalItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

