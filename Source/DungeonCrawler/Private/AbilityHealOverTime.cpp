// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHealOverTime.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "TimerManager.h"

void UAbilityHealOverTime::execute_Implementation()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        if (MontageToPlay)
            player->PlayAnimMontage(MontageToPlay, 1.f, "HoT");
    }

}

bool UAbilityHealOverTime::bShouldExecute_Implementation()
{
    if(APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        UWorld* World = player->GetWorld();
        return World && !player->GetWorldTimerManager().IsTimerActive(FCooldownTimer) && player->CanPlayerDoAction(EResourceTypes::Mana, 0);
    }
    return false;
}

void UAbilityHealOverTime::Logic()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        UWorld* World = player->GetWorld();
        if (!World) return;
        playerWorld = World;
        if (player->GetWorldTimerManager().IsTimerActive(FHealTimer)) {
            player->GetWorldTimerManager().ClearTimer(FHealTimer);
        }
        if (player->GetWorldTimerManager().IsTimerActive(FInnerHealTimer)) {
            player->GetWorldTimerManager().ClearTimer(FInnerHealTimer);
        }
        if (player->GetWorldTimerManager().IsTimerActive(FCooldownTimer)) {
            player->GetWorldTimerManager().ClearTimer(FCooldownTimer);
        }
        player->GetWorldTimerManager().SetTimer(FHealTimer, [this,player]() {
            if (player) {
                player->GetWorldTimerManager().ClearTimer(FInnerHealTimer);
            }
        }, AbilityTime, false);
        player->GetWorldTimerManager().SetTimer(FInnerHealTimer, [this, player]()
        {
            if (!player) return;
            if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
            {
                if (PCS->playerStats.currentHealth >= PCS->GetTotalHealth())
                {
                    player->GetWorldTimerManager().ClearTimer(FInnerHealTimer);
                }
                PCS->playerStats.currentHealth += HealAmount;
                PCS->playerStats.currentHealth = FMath::Clamp(PCS->playerStats.currentHealth, 0.f, PCS->GetTotalHealth());
                PCS->playerStats.currentMana += HealAmount;
                PCS->playerStats.currentMana = FMath::Clamp(PCS->playerStats.currentMana, 0.f, PCS->GetTotalMana());
                if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
                    if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
                    {
                        if (PlayerHUD->GetMainUIWidget() && PCS->GetTotalHealth() > 0.f && PCS->GetTotalMana() > 0.f)
                        {
                            PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
                            PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Mana", PCS->playerStats.currentMana / PCS->GetTotalMana());
                        }
                    }
                }
            }
        }, HealRate, true);
        player->GetWorldTimerManager().SetTimer(FCooldownTimer, CooldownTime, false);
    }
}

void UAbilityHealOverTime::BeginDestroy()
{
    Super::BeginDestroy();
    if (playerWorld)
    {
        FTimerManager& TimerManager = playerWorld->GetTimerManager();
        if (TimerManager.IsTimerActive(FHealTimer)) {
            TimerManager.ClearTimer(FHealTimer);
        }
        if (TimerManager.IsTimerActive(FInnerHealTimer)) {
            TimerManager.ClearTimer(FInnerHealTimer);
        }
        if (TimerManager.IsTimerActive(FCooldownTimer)) {
            TimerManager.ClearTimer(FCooldownTimer);
        }
    }
}
