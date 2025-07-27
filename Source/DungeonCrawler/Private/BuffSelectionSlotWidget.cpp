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
		if (BuffName) {
			BuffName->SetText(BuffDataAsset->BuffName);
		}
		if (DisplayingImage && BuffDataAsset->ImageToShow) {
			DisplayingImage->SetBrushFromTexture(BuffDataAsset->ImageToShow);
		}
		UWorld* World = GetWorld();
		if (World) {
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
			{
				if (MGI->localPlayer)
				{
					if (APlayerController* PC = Cast<APlayerController>(MGI->localPlayer->GetController()))
					{
						if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
						{
							OwningWidget = InGamePlayerHUD->BuffSelectionWidget;
							if (OwningWidget) {
								for(UBuffDataAsset*Buff : OwningWidget->ChosenBuffs)
								{
									if(Buff && Buff->BuffName.EqualTo(BuffData->BuffName))
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
		if (BuffName) {
			BuffName->SetText(BuffData->BuffName);
		}
		if (DisplayingImage && BuffData->ImageToShow) {
			DisplayingImage->SetBrushFromTexture(BuffData->ImageToShow);
		}
	}
}

void UBuffSelectionSlotWidget::RollDrop()
{

	if (OwningWidget) {
		AllBuffs = OwningWidget->AllBuffs;
		UWorld* World = GetWorld();
		if (!World) return;
		if (AllBuffs.Num() == 0) return;
		World->GetTimerManager().SetTimer(FRollingTimer, [this]()
			{
				if (AllBuffs.Num() == 0) return;
				int RandomRolledNum = FMath::RandRange(0, AllBuffs.Num() - 1);
				if (AllBuffs.IsValidIndex(RandomRolledNum)) {
					SafeDataAsset = AllBuffs[RandomRolledNum];
				}
				ResetData();
			}, 0.05, true);
		World->GetTimerManager().SetTimer(FStopTimer, [this]()
			{
				UWorld* World = GetWorld();
				if (World && World->GetTimerManager().IsTimerActive(FRollingTimer))
				{
					SafeDataAsset = ChoseDataAssetSafe;
					ResetData();
					World->GetTimerManager().ClearTimer(FRollingTimer);
				}
			}, 1.5f, false);
	}
}
