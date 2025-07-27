// Fill out your copyright notice in the Description page of Project Settings.

#include "BlacksmithUpgradeUI.h"

#include "BlacksmithItemWidget.h"
#include "GearItemInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterController.h"
#include "Weapon.h"
#include "PlayerCharacterState.h"
#include "WeaponInstance.h"
#include "Components/Button.h"

void UBlacksmithUpgradeUI::SetUpgradeItemData(UObject* SelectedItem)
{
    if(UItemDataObject*SelectedItemDO = Cast<UItemDataObject>(SelectedItem))
    {
        UpgradeItemData = SelectedItemDO;
    }
}

void UBlacksmithUpgradeUI::NativeConstruct()
{
    Super::NativeConstruct();
    if(ItemListView)
    {
        ItemListView->OnItemClicked().AddUObject(this,&UBlacksmithUpgradeUI::SetUpgradeItemData);
        ItemListView->OnEntryWidgetGenerated().AddUObject(this, &UBlacksmithUpgradeUI::HandleOnEntryWidgetGenerated);
    }
    if(UpgradeButton)
    {
        UpgradeButton->OnClicked.AddDynamic(this, &UBlacksmithUpgradeUI::OnButtonClicked);
    }
    if(CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UBlacksmithUpgradeUI::OnCloseButtonClicked);
    }
    InitializeList();
}

void UBlacksmithUpgradeUI::HandleOnEntryWidgetGenerated(UUserWidget& GeneratedWidget)
{
    if(UBlacksmithItemWidget*BlacksmithItemWidget = Cast<UBlacksmithItemWidget>(&GeneratedWidget))
    {
        BlacksmithItemWidget->SetParentWidget(this);
    }
}

void UBlacksmithUpgradeUI::OnButtonClicked()
{
    bool bRefresh = false;
    if(!UpgradeItemData || !ItemListView) return;
    if(UWeaponInstance*WeaponInstance = Cast<UWeaponInstance>(UpgradeItemData))
    {
        if (WeaponInstance->bCanUpgrade()) {
            WeaponInstance->UpgradeWeapon();
            UE_LOG(LogTemp, Warning, TEXT("ITEM UPGRADED"));
            bRefresh = true;
        }
    }
    if(UGearItemInstance*GearItemInstance = Cast<UGearItemInstance>(UpgradeItemData))
    {
        if (GearItemInstance->bCanUpgrade()) {
            GearItemInstance->UpgradeGear();
            UE_LOG(LogTemp, Warning, TEXT("ITEM UPGRADED"));
            bRefresh = true;
        }
    }
    if (bRefresh && GetWorld()) {
        ItemListView->ClearListItems();
        FTimerHandle FDelay;
        GetWorld()->GetTimerManager().SetTimer(FDelay, [this]() {
            InitializeList();
        }, 0.1f, false);
        UpgradeItemData = nullptr;
    }
}

void UBlacksmithUpgradeUI::OnCloseButtonClicked()
{
    APawn* OwningPawn = GetOwningPlayerPawn();
    if (!OwningPawn) return;
    if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(OwningPawn->GetController()))
    {
        PCC->ResetCursor();
        SetVisibility(ESlateVisibility::Hidden);
    }
}

void UBlacksmithUpgradeUI::InitializeList()
{
    APawn* OwningPawn = GetOwningPlayerPawn();
    if (!OwningPawn || !ItemListView) return;
    if (APlayerCharacter* player = Cast<APlayerCharacter>(OwningPawn)) {
        if (APlayerCharacterState* pcs = player->GetPlayerCharacterState()) {
            if (pcs->PlayerGear.Contains(EGearType::RHand))
            {
                if (AWeapon* Weapon = Cast<AWeapon>(*pcs->PlayerGear.Find(EGearType::RHand)))
                {
                    if (Weapon->WeaponInstance)
                    {
                        ItemListView->AddItem(Weapon->WeaponInstance);
                    }
                }
            }
            for (UItemDataObject* item : pcs->Inventory)
            {
                if (item && (Cast<UWeaponInstance>(item) || Cast<UGearItemInstance>(item)))
                {
                    ItemListView->AddItem(item);
                }
            }
        }
    }
}
