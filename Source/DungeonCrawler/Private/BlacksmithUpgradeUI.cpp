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

	if (bRefresh) {
		ItemListView->ClearListItems();
		FTimerHandle FDelay;
		GetWorld()->GetTimerManager().SetTimer(FDelay, [this]() {	InitializeList();
			}, 0.1f, false);
		UpgradeItemData = nullptr;
	}
}

void UBlacksmithUpgradeUI::OnCloseButtonClicked()
{
	if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(GetOwningPlayerPawn()->GetController()))
	{
		PCC->ResetCursor();
		SetVisibility(ESlateVisibility::Hidden);

	}
}

void UBlacksmithUpgradeUI::InitializeList()
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwningPlayerPawn())) {
		if (APlayerCharacterState* pcs = player->GetPlayerCharacterState()) {

			if (AWeapon* Weapon = Cast<AWeapon>(*pcs->PlayerGear.Find(EGearType::RHand)))
			{
				ItemListView->AddItem(Weapon->WeaponInstance);
			}
			for (UItemDataObject* item : pcs->Inventory)
			{
				if (Cast<UWeaponInstance>(item) || Cast<UGearItemInstance>(item))
				{
					ItemListView->AddItem(item);
				}
			}
		}
	}
}
