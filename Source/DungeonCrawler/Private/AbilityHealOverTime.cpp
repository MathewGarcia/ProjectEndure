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
		player->PlayAnimMontage(MontageToPlay, 1.f, "HoT");
	}

}

bool UAbilityHealOverTime::bShouldExecute_Implementation()
{
	if(APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
	{
		return !player->GetWorldTimerManager().IsTimerActive(FCooldownTimer) && player->CanPlayerDoAction(EResourceTypes::Mana, 0);
	}
	return false;
}

void UAbilityHealOverTime::Logic()
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
	{

		playerWorld = player->GetWorld();

		player->GetWorldTimerManager().SetTimer(FHealTimer, [this,player]()
		{
			player->GetWorldTimerManager().ClearTimer(FInnerHealTimer);
		
		}, AbilityTime, false);

		player->GetWorldTimerManager().SetTimer(FInnerHealTimer, [this, player]()
			{

				if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
				{
					if (PCS->playerStats.currentHealth >= PCS->GetTotalHealth())
					{
						player->GetWorldTimerManager().ClearTimer(FInnerHealTimer);

					}
					PCS->playerStats.currentHealth += HealAmount;
					PCS->playerStats.currentHealth = FMath::Clamp(PCS->playerStats.currentHealth, 0, PCS->GetTotalHealth());

					if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
						if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
						{
							if (PlayerHUD->GetMainUIWidget())
							{
								PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
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

	if (playerWorld) {
		playerWorld->GetTimerManager().ClearTimer(FHealTimer);
		playerWorld->GetTimerManager().ClearTimer(FInnerHealTimer);
		playerWorld->GetTimerManager().ClearTimer(FCooldownTimer);
	}
}
