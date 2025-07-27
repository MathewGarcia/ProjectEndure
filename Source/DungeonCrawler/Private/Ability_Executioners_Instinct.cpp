// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Executioners_Instinct.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"


void UAbility_Executioners_Instinct::execute_Implementation()
{
	Super::execute_Implementation();


	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->OnEnemyDamageTaken.AddLambda([this](AEnemy* Enemy,float& Damage)
			{
				if (!Enemy || Enemy->MaxHealth <= 0.f) return;
				float RemainingHealth = Enemy->Health / Enemy->MaxHealth;
				if (RemainingHealth <= 0.2f)
				{
					Damage *= 1.25;
				}

			});
		}
	}
}

bool UAbility_Executioners_Instinct::bShouldExecute_Implementation()
{

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		return PCS->LearnedAbilities.Contains(this);
	}

	return false;
}
