// Fill out your copyright notice in the Description page of Project Settings.


#include "Drink_Heal.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "TimerManager.h"

void UDrink_Heal::execute_Implementation()
{
    if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOuter()))
    {
        if (MontageToPlay)
        {
            player->PlayAnimMontage(MontageToPlay, 1, "Drink Heal");
        }
    }
}

bool UDrink_Heal::bShouldExecute_Implementation()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        return !player->GetWorldTimerManager().IsTimerActive(FCooldownTimer) && player->CanPlayerDoAction(EResourceTypes::Mana,0);
    }
    return false;
}

void UDrink_Heal::Logic()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
        {
            PCS->playerStats.currentHealth += HealAmount;
            PCS->playerStats.currentMana += HealAmount;
            if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
                if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
                {
                    if (UMainPlayerWidget* MainUIWidget = PlayerHUD->GetMainUIWidget())
                    {
                        float healthRatio = PCS->GetTotalHealth() > 0 ? PCS->playerStats.currentHealth / PCS->GetTotalHealth() : 0.f;
                        float manaRatio = PCS->GetTotalMana() > 0 ? PCS->playerStats.currentMana / PCS->GetTotalMana() : 0.f;
                        MainUIWidget->UpdateProgressBar("Health", healthRatio);
                        MainUIWidget->UpdateProgressBar("Mana", manaRatio);
                    }
                }
            }
            player->GetWorldTimerManager().SetTimer(FCooldownTimer, [this]() {}, CooldownTime, false);
        }
    }
}
