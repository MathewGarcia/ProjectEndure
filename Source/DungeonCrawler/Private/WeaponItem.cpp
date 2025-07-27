// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "Weapon.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

//deprecated
void UWeaponItem::useItem_Implementation(APlayerCharacter* player)
{
    if (!player) return;
    UWorld* World = GetWorld();
    if (!World) return;
    if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
        if (WeaponItemData->ItemToSpawn) {
            if (AWeapon* SpawnedWeapon = World->SpawnActor<AWeapon>(WeaponItemData->ItemToSpawn)) {
                SpawnedWeapon->ItemData = WeaponItemData;
                player->tryEquipGear(SpawnedWeapon, WeaponItemData->GearType, ItemDataObject);
            }
        }
    }
}
