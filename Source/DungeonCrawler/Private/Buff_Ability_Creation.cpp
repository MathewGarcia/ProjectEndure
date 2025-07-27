// Fill out your copyright notice in the Description page of Project Settings.


#include "Buff_Ability_Creation.h"

#include "Ability.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"


void UBuff_Ability_Creation::Execute()
{

	if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
	{
		if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
		{
			if (AbilityToSpawn)
			{
				if (UAbility* Ability = NewObject<UAbility>(PCS, AbilityToSpawn))
				{
					Ability->AbilityActivationType = EAbilityActivationType::Event;
					//int Pos = PCS->LearnedAbilities.Find(Ability);
					PCS->LearnedAbilities.AddUnique(Ability);
				}
			}
		}
	}
}
