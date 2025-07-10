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
		player->PlayAnimMontage(MontageToPlay, 1, "Drink Heal");
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

			if (APlayerController* PC = Cast<APlayerController>(player->GetController())) {
				if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD()))
				{
					if (PlayerHUD->GetMainUIWidget())
					{
						PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
					}
				}
			}
			player->GetWorldTimerManager().SetTimer(FCooldownTimer, [this]()
				{

				}, CooldownTime, false);
		}
	}
}
