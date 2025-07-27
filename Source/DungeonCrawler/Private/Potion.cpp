// Fill out your copyright notice in the Description page of Project Settings.

#include "Potion.h"

#include "ConsumeableItemData.h"
#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UPotion::useItem_Implementation(APlayerCharacter* player)
{
    if (!player) return;
    UWorld* World = GetWorld();
    if (!World) return;
    if(World->GetTimerManager().IsTimerActive(BuffTimerHandle))
    {
        return;
    }
    if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
        if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(player->GetPlayerState())) {
            if (UConsumeableItemData* CID = Cast<UConsumeableItemData>(ItemData))
            {
                bool ActivateTimer = false;
                if (CID->Health > 0)
                {
                    PCS->playerStats.currentHealth += CID->Health;
                    if (AInGamePlayerHUD* PlayerHUD= Cast<AInGamePlayerHUD>(PC->GetHUD()))
                    {
                        TWeakObjectPtr<UMainPlayerWidget> MPWPtr = PlayerHUD->GetMainUIWidget();
                        if (MPWPtr.IsValid())
                        {
                            UMainPlayerWidget* MPW = MPWPtr.Get();
                            if (MPW && PCS->GetTotalHealth() > 0) {
                                MPW->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
                            }
                        }
                    }
                }
                if (CID->Intellect > 0)
                {
                    PCS->playerStats.totalIntellect += CID->Intellect;
                    ActivateTimer = true;
                }
                if(CID->Strength > 0)
                {
                    PCS->playerStats.totalStrength += CID->Strength;
                    ActivateTimer = true;
                }
                if(ActivateTimer)
                {
                    TWeakObjectPtr<APlayerCharacterState> SafePCS = PCS;
                    TWeakObjectPtr<UConsumeableItemData> SafeConsumeableItemData = CID;
                    World->GetTimerManager().SetTimer(BuffTimerHandle, [this, SafePCS, SafeConsumeableItemData]()
                        {
                            if (!SafePCS.IsValid() || !SafeConsumeableItemData.IsValid())
                            {
                                UE_LOG(LogTemp, Warning, TEXT("Safe calls failed."));
                                return;
                            }
                            APlayerCharacterState* localPCS = SafePCS.Get();
                            UConsumeableItemData* ConsumeableItemData = SafeConsumeableItemData.Get();
                            if (!localPCS || !ConsumeableItemData) return;
                            localPCS->playerStats.totalIntellect -= ConsumeableItemData->Intellect;
                            localPCS->playerStats.totalStrength -= ConsumeableItemData->Strength;
                    }, time, false);
                }
            }
        }
    }
}
