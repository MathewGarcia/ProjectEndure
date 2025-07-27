// Fill out your copyright notice in the Description page of Project Settings.


#include "GearSlotWidget.h"

#include "ItemDataObject.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UGearSlotWidget::UpdateGearSlot(UItemDataObject* newItemData)
{
	if (!newItemData) return;

	itemDataObject = newItemData;
	itemData = newItemData->ItemData;
	if (ItemImage && itemData && itemData->Image) {
		ItemImage->SetBrushFromTexture(itemData->Image);
	}
}

void UGearSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (GearButton) {
		GearButton->OnClicked.AddUniqueDynamic(this,&UGearSlotWidget::ResetGearSlot);
	}
}

void UGearSlotWidget::ResetGearSlot()
{
	if (!itemData || !itemDataObject) return;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		if(player->GetPlayerCharacterState())
		{
			player->GetPlayerCharacterState()->AddItemToInventory(itemDataObject);
		}
		if(itemData->ItemToSpawn)
		{
			if(AItem*Item = itemData->ItemToSpawn->GetDefaultObject<AItem>())
			{
				player->UnEquipGear(Item);
			}
		}
	}
	itemData = nullptr;
	if (ItemImage) {
		ItemImage->SetBrushFromTexture(nullptr);
	}
}
