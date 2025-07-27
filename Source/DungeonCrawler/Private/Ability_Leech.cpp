// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Leech.h"

#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacterState.h"

UAbility_Leech::UAbility_Leech(): DamageDealt(0)
{
	AbilityActivationType = EAbilityActivationType::Event;
}

void UAbility_Leech::execute_Implementation()
{
	Super::execute_Implementation();

	float ReturnedHealth = DamageDealt * Percent;

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{

		if (APlayerController* PC = PCS->GetPlayerController()) {
			if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD())) {
				if (PlayerHUD->GetMainUIWidget() && PCS->GetTotalHealth() > 0.f)
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
