// Fill out your copyright notice in the Description page of Project Settings.


#include "Buff_Cooldown_Reduction.h"

#include "Ability.h"
#include "PlayerCharacterState.h"

void UBuff_Cooldown_Reduction::Execute()
{
	Super::Execute();

	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		for(UAbility*Ability : PCS->LearnedAbilities)
		{
			//reduce cooldown time by 10%
			if (Ability->AbilityActivationType == EAbilityActivationType::Activate && Ability->Cooldown > 0) {
				Ability->Cooldown = FMath::Max(Ability->Cooldown * 0.9f, Ability->MinCooldownTime);
			}
		}
	}
}
