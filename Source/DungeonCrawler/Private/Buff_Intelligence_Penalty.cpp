// Fill out your copyright notice in the Description page of Project Settings.


#include "Buff_Intelligence_Penalty.h"

#include "Ability.h"
#include "Ability_Intelligence_Penalty.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"

void UBuff_Intelligence_Penalty::Execute() {


	if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOuter()))
	{
		if(APlayerCharacterState*PCS = player->GetPlayerCharacterState())
		{
			if(UAbility*IntelPen = NewObject<UAbility>(PCS,AbilityIntellClass))
			{
				IntelPen->AbilityActivationType = EAbilityActivationType::Event;
				int Pos = PCS->LearnedAbilities.Find(IntelPen);
				//if we find it, just increase the amount
				if (Pos != INDEX_NONE)
				{
					if(UAbility_Intelligence_Penalty*FoundIntelPenn = Cast<UAbility_Intelligence_Penalty>(PCS->LearnedAbilities[Pos]))
					{
						++FoundIntelPenn->Amount;
					}
				}
				else
				{
					PCS->LearnedAbilities.Add(IntelPen);
				}
			}
		}
	}


}
