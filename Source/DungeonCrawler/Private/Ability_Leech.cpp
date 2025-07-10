// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Leech.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacterState.h"

void UAbility_Leech::execute_Implementation()
{
	Super::execute_Implementation();

	float ReturnedHealth = DamageDealt * Percent;

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{

		if (APlayerController* PC = PCS->GetPlayerController()) {
			if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD())) {
				if (PlayerHUD->GetMainUIWidget())
				{
					PCS->playerStats.currentHealth += ReturnedHealth;
					PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
				}
				
			}
		}
	}

}

bool UAbility_Leech::bShouldExecute_Implementation()
{

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		return PCS->LearnedAbilities.Contains(this);
	}

	return false;
}
