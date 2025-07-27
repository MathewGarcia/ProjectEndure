// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Reflect_Damage.h"

#include "Enemy.h"
#include "PlayerCharacterState.h"
#include "Engine/DamageEvents.h"

void UAbility_Reflect_Damage::execute_Implementation()
{
	Super::execute_Implementation();

	float ReflectDamage = DamageTaken * Percent;
	if (DamageCauser)
	{
		DamageCauser->TakeDamage(ReflectDamage, FDamageEvent(), nullptr, nullptr);
	}
}

bool UAbility_Reflect_Damage::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		return PCS->LearnedAbilities.Contains(this);
	}

	return false;
}
