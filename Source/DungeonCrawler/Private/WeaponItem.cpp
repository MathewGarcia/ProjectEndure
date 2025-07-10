// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponItem.h"
#include "Weapon.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

//deprecated
void UWeaponItem::useItem_Implementation(APlayerCharacter* player)
{

	if (!player) return;

	if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {

		if (AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponItemData->ItemToSpawn)) {
			SpawnedWeapon->ItemData = WeaponItemData;
			player->tryEquipGear(SpawnedWeapon, WeaponItemData->GearType, ItemDataObject);
		}
	}
}
