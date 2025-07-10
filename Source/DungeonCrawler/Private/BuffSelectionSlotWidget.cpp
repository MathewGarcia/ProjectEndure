// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSelectionSlotWidget.h"

#include "BuffDataAsset.h"
#include "BuffSelectionWidget.h"
#include "InGamePlayerHUD.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBuffSelectionSlotWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (!ListItemObject) return;

	if(UBuffDataAsset* BuffDataAsset = Cast<UBuffDataAsset>(ListItemObject))
	{
		BuffData = BuffDataAsset;
		BuffName->SetText(BuffDataAsset->BuffName);
		DisplayingImage->SetBrushFromTexture(BuffDataAsset->ImageToShow);
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (MGI->localPlayer)
			{
				if (APlayerController* PC = Cast<APlayerController>(MGI->localPlayer->GetController()))
				{
					if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
					{
						OwningWidget = InGamePlayerHUD->BuffSelectionWidget;

						for(UBuffDataAsset*Buff : OwningWidget->ChosenBuffs)
						{
							if(Buff->BuffName.EqualTo(BuffData->BuffName))
							{
								ChoseDataAssetSafe = Buff;
								break;
							}
						}
						RollDrop();

					}
				}
			}
		}		
	}
}

void UBuffSelectionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UBuffSelectionSlotWidget::ResetData()
{
	if (!SafeDataAsset.IsValid()) return;

	BuffData = SafeDataAsset.Get();

	if (BuffData)
	{
		BuffName->SetText(BuffData->BuffName);
		DisplayingImage->SetBrushFromTexture(BuffData->ImageToShow);
	}
}

void UBuffSelectionSlotWidget::RollDrop()
{

	if (OwningWidget) {

		AllBuffs = OwningWidget->AllBuffs;
		GetWorld()->GetTimerManager().SetTimer(FRollingTimer, [this]()
			{

				//keep selecting a random one.
				int RandomRolledNum = FMath::RandRange(0, AllBuffs.Num() - 1);

				if (AllBuffs.IsValidIndex(RandomRolledNum)) {
					SafeDataAsset = AllBuffs[RandomRolledNum];
				}

				ResetData();


			}, 0.05, true);



		GetWorld()->GetTimerManager().SetTimer(FStopTimer, [this]()
			{

				if (GetWorld()->GetTimerManager().IsTimerActive(FRollingTimer))
				{

					SafeDataAsset = ChoseDataAssetSafe;
					ResetData();

					GetWorld()->GetTimerManager().ClearTimer(FRollingTimer);

				}

			}, 1.5f, false);
	}
}
