// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Greater_Orb.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"

void UAbility_Greater_Orb::execute_Implementation()
{
	Super::execute_Implementation();
	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (MontageToPlay)
				player->PlayAnimMontage(MontageToPlay, player->MontageSpeed);
		}
	}

}

bool UAbility_Greater_Orb::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return false;
}

void UAbility_Greater_Orb::Logic()
{
	Super::Logic();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = player->GetWorld();
			if (!World) return;
			if(AWeapon*CurrentWeapon = player->GetCurrentWeapon())
			{
				if (CurrentWeapon->Projectile) {
					player->SpawnProjectile(FVector(2.f),1.5f);
					if (World->GetTimerManager().IsTimerActive(FCooldown)) {
						World->GetTimerManager().ClearTimer(FCooldown);
					}
					World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
				}
			}
		}
	}
}
