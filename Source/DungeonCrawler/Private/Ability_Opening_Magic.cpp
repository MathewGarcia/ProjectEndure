// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Opening_Magic.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UAbility_Opening_Magic::execute_Implementation()
{
	Super::execute_Implementation();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {

			player->OnEnemyDamageTaken.Remove(FOpeningMagic);

			FOpeningMagic = player->OnEnemyDamageTaken.AddLambda([this](AEnemy* Enemy, float& Damage)
			{
				float Percent = Enemy->Health / Enemy->MaxHealth;
				if(Percent >= 1.f)
				{
					Damage *= 1.25;
				}
			});
		}
	}

}

bool UAbility_Opening_Magic::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			return PCS->LearnedAbilities.Contains(this);
		}
	}
	return false;
}
