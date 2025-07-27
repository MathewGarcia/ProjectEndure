// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Elemental_Barrage.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"

void UAbility_Elemental_Barrage::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			currentAmount = 0;
			if (MontageToPlay)
				player->PlayAnimMontage(MontageToPlay, player->MontageSpeed);
		}
	}
}

bool UAbility_Elemental_Barrage::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
		}
	}
	return false;
}

void UAbility_Elemental_Barrage::Logic()
{
	Super::Logic();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = player->GetWorld();
			if (!World) return;
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			if (AWeapon* CurrentWeapon = player->GetCurrentWeapon())
			{
				if (CurrentWeapon->Projectile) {
					if (World->GetTimerManager().IsTimerActive(FFireRate)) {
						World->GetTimerManager().ClearTimer(FFireRate);
					}
					if (World->GetTimerManager().IsTimerActive(FCooldown)) {
						World->GetTimerManager().ClearTimer(FCooldown);
					}
					World->GetTimerManager().SetTimer(FFireRate, [this, safePlayer, World]
					{
							if (!safePlayer.IsValid()) return;

						if(currentAmount >= MaxToSpawn)
						{
							if (World) World->GetTimerManager().ClearTimer(FFireRate);
							return;
						}

							if (APlayerCharacter* localPlayer = safePlayer.Get()) {
								localPlayer->SpawnProjectile(FVector(1.f), 0.5f,EProjectileType::AoE);
								++currentAmount;
							}

					}, SpawnRate, true);
					World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
				}
			}
		}
	}
}
