// Fill out your copyright notice in the Description page of Project Settings.


#include "GearItem.h"

#include "GearArmorItem.h"
#include "Item.h"
#include "ItemData.h"
#include "PlayerCharacter.h"

void UGearItem::useItem_Implementation(APlayerCharacter* player) {

	if(UGearItemData*GearItemData = Cast<UGearItemData>(ItemData))
	{
		if (AGearArmorItem* ArmorItem = GearItemData->ItemToSpawn->GetDefaultObject<AGearArmorItem>())
		{
				player->tryEquipGear(ArmorItem, GearItemData->GearType,ItemDataObject);
		}
	}


}
