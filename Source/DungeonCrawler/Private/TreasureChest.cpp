// Fill out your copyright notice in the Description page of Project Settings.


#include "TreasureChest.h"

#include "GearArmorItem.h"
#include "Item.h"
#include "NiagaraComponent.h"
#include "Weapon.h"
#include "WeaponInstance.h"
#include "Components/BoxComponent.h"


void ATreasureChest::Interact_Implementation()
{
	TSubclassOf<AItem> ChosenItem = RollForItemToSpawn();

	if(ChosenItem)
	{
		if (OpenAnimation) {
			SkeletalMesh->PlayAnimation(OpenAnimation, false);
		}

		if(NiagaraComponent)
		{
			NiagaraComponent->SetFloatParameter("User.Velocity", 1000.f);
			NiagaraComponent->Activate(true);
		}

		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if(AItem*ItemSpawned = GetWorld()->SpawnActor<AItem>(ChosenItem, GetActorLocation(),FRotator(0.f),params))
		{
			StartLocation = GetActorLocation();
			EndLocation = StartLocation + (GetActorRightVector() * 100.f);

			SpawnedItem = ItemSpawned;

			if(AWeapon*weapon = Cast<AWeapon>(SpawnedItem))
			{
				if(UWeaponInstance*WI = weapon->WeaponInstance)
				{
					if (weapon->WeaponType != EWeaponType::Mage) {
						WI->RandomlySetElementType();
					}

					WI->InitAccordingToLevel(FMath::RandRange(0, 3));
				}
			}
			else if (AGearArmorItem*Gear = Cast<AGearArmorItem>(SpawnedItem))
			{
				Gear->InitGearItemInstance(FMath::RandRange(0, 3));
			}

			//Destroy the box collision.
			BoxComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

			bIsMoving = true;
			FTimerHandle TimerHandle;

			GetWorldTimerManager().SetTimer(TimerHandle, [this]()
			{
				if(NiagaraComponent)
					NiagaraComponent->DestroyComponent();

			}, 0.3, false);
		}
	}
}

TSubclassOf<AItem> ATreasureChest::RollForItemToSpawn()
{
	float total = 0.f;

	for(const auto& pair : ItemToSpawn)
	{
		total += pair.Value;
	}

	float RolledNum = FMath::FRandRange(0, total);

	for (const auto& pair : ItemToSpawn)
	{
		RolledNum -= pair.Value;

		if(RolledNum <= 0)
		{
			return pair.Key;
		}
	}

	return nullptr; //should never happen
}

void ATreasureChest::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(bIsMoving && SpawnedItem)
	{
		float Duration = 1.f;
		currentAlpha += (DeltaSeconds / Duration);

		if(currentAlpha >= 1)
		{
			currentAlpha = 1.f;
			bIsMoving = false;
		}
		FVector NewLocation = FMath::Lerp(StartLocation, EndLocation, currentAlpha);

		float ArcOffset = ArcHeight * FMath::Sin(PI * currentAlpha);
		NewLocation.Z += ArcOffset;

		SpawnedItem->SetActorLocation(NewLocation);
	}
}
