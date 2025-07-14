// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Second_Wind.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacterState.h"

UAbility_Second_Wind::UAbility_Second_Wind()
{
	AbilityActivationType = EAbilityActivationType::Event;
}

void UAbility_Second_Wind::execute_Implementation()
{
	Super::execute_Implementation();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{

		if (APlayerController* PC = PCS->GetPlayerController()) {
			if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD())) {
				if (PlayerHUD->GetMainUIWidget())
				{
					PCS->playerStats.currentHealth += PCS->GetTotalHealth() / 2;
					PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
					PCS->LearnedAbilities.Remove(this);

				}

			}
		}
	}

}

bool UAbility_Second_Wind::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		return PCS->LearnedAbilities.Contains(this);
	}

	return false;
}
