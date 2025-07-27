// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Crushing_Resolve.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Crushing_Resolve::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (!player) return;
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			TWeakObjectPtr<APlayerCharacterState> safePCS = PCS;
			FCrushingResolveDelegateHandle = player->OnPlayerDamaged.AddLambda([this,safePlayer,safePCS](float& Damage, const FDamageEvent& DamageEvent, AController* Controller, AActor* DamageCauser)
				{
					if (!safePlayer.IsValid() || !safePCS.IsValid()) return;
					if (APlayerCharacterState* localPCS = Cast<APlayerCharacterState>(safePCS.Get())) {
						float TotalHealth = localPCS->GetTotalHealth();
						if (TotalHealth <= 0.f) return;
						float RemainingHealth = localPCS->playerStats.currentHealth / TotalHealth;
						if (RemainingHealth <= 0.3) {
							Damage -= Damage * 0.1;
						}
					}
			});
		}
	}

}

bool UAbility_Crushing_Resolve::bShouldExecute_Implementation()
{

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		return PCS->LearnedAbilities.Contains(this);
	}

	return false;
}
