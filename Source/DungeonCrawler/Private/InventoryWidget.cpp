// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "InventoryItemWidget.h"
#include "PlayerCharacterState.h"
#include "ItemData.h"
#include "ItemDataObject.h"
#include "Components/TileView.h"

void UInventoryWidget::UpdateInventory(TArray<UItemDataObject*> Inventory)
{
	if (!InventoryTileView) return;

	UE_LOG(LogTemp, Warning, TEXT("Update inventory"));


	InventoryTileView->ClearListItems();

	for (UItemDataObject* item : Inventory)
	{
		if (!InventoryItemWidgetClass) return;

		if (!item || !item->ItemData) continue;

		InventoryTileView->AddItem(item);
	}
	

}


void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOwningPlayer()->GetPlayerState<APlayerCharacterState>()))
	{
		pcs->FInventoryUpdated.AddUniqueDynamic(this, &UInventoryWidget::UpdateInventory);
	
		UpdateInventory(pcs->Inventory);

		UE_LOG(LogTemp, Warning, TEXT("Inventory Widget ready"));


	}
}
