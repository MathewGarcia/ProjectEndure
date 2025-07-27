// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemWidget.h"

#include "BaseItem.h"
#include "GearItemData.h"
#include "InGamePlayerHUD.h"
#include "ItemData.h"
#include "ItemDataObject.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "WeaponItemData.h"
#include "Widget_GearInfo.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

int UInventoryItemWidget::GetQuantity()
{
    if(itemDataObject)
    {
        return itemDataObject->Quantity;
    }
    return 0;
}

void UInventoryItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
    if (UItemDataObject* DataObj = Cast<UItemDataObject>(ListItemObject))
    {
        UE_LOG(LogTemp, Warning, TEXT("LIST ITEM OBJECT: %s"), *ListItemObject->GetName());
        if (!DataObj->ItemData) return;
        UItemDataAsset* ItemData = DataObj->ItemData;
        itemData = ItemData;
        itemDataObject = DataObj;
        itemDataObject->OnItemDataChanged.AddUniqueDynamic(this, &UInventoryItemWidget::RefreshText);
        if (ItemNameText) {
            ItemNameText->SetText(ItemData->ItemName);
        }
        if (QuantityText) {
            QuantityText->SetText(FText::AsNumber(itemDataObject->Quantity));
        }
        if (ItemPic && ItemData->Image) {
            ItemPic->SetBrushFromTexture(ItemData->Image);
        }
        if(APlayerController*PC= GetOwningPlayer())
        {
            if(AInGamePlayerHUD*PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
            {
               if(UMainPlayerWidget*MPW =  PlayerHUD->GetMainUIWidget())
               {
                   GearInfoWidget = MPW->GearInfo;
               }
            }
        }
    }
    if(ItemButton)
    {
        ItemButton->OnClicked.AddUniqueDynamic(this, &UInventoryItemWidget::OnItemClicked);
        ItemButton->OnHovered.AddUniqueDynamic(this, &UInventoryItemWidget::OnItemHovered);
    }
}

void UInventoryItemWidget::RefreshText()
{
    if (itemDataObject && QuantityText) {
        QuantityText->SetText(FText::AsNumber(itemDataObject->Quantity));
    }
}

void UInventoryItemWidget::OnItemClicked()
{
    if (!itemData || !itemDataObject) return;
    UE_LOG(LogTemp, Warning, TEXT("Item clicked using item %s"), *itemData->ItemName.ToString());
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwningPlayerPawn())){
        if (itemDataObject->ItemData && itemDataObject->ItemData->ItemLogicClass) {
            if (UWeaponItemData* WID = Cast<UWeaponItemData>(itemData)) {
                if (APlayerCharacterState* localPCS = player->GetPlayerCharacterState()) {
                    if (localPCS->playerStats.Strength < WID->ReqStrength || localPCS->playerStats.Intellect < WID->ReqIntellect)
                    {
                        return;
                    }
                }
            }
            else if (UGearItemData* GID = Cast<UGearItemData>(itemData)) {
                if (APlayerCharacterState* localPCS = player->GetPlayerCharacterState()) {
                    if (localPCS->playerStats.Strength < GID->ReqStrength || localPCS->playerStats.Intellect < GID->ReqIntellect)
                    {
                        return;
                    }
                }
            }
            player->UseItem(itemDataObject);
        }
    }
}

void UInventoryItemWidget::OnItemHovered()
{
    UE_LOG(LogTemp, Warning, TEXT("Item hovered"));
    if (!GearInfoWidget || !itemDataObject) return;
    if(GearInfoWidget->GetVisibility() == ESlateVisibility::Collapsed || GearInfoWidget->GetVisibility() == ESlateVisibility::Hidden)
    {
        GearInfoWidget->SetVisibility(ESlateVisibility::Visible);
    }
    GearInfoWidget->SetInfo(itemDataObject);
}

void UInventoryItemWidget::NativeConstruct()
{
    Super::NativeConstruct();
}
