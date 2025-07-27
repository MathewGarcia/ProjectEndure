// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Floating_Mine.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"

void UAbility_Floating_Mine::execute_Implementation()
{
	Super::execute_Implementation();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (MontageToPlay)
				player->PlayAnimMontage(MontageToPlay, player->MontageSpeed);
		}
	}
}

bool UAbility_Floating_Mine::bShouldExecute_Implementation()
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

void UAbility_Floating_Mine::Logic()
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
					SafeProjectile = player->SpawnProjectile(FVector(8.f), 0.5f, EProjectileType::AoE, 0.5, 0.f);
					if (World->GetTimerManager().IsTimerActive(FExplosionTimer)) {
						World->GetTimerManager().ClearTimer(FExplosionTimer);
					}
					World->GetTimerManager().SetTimer(FExplosionTimer, [this, safePlayer]
						{
							if (!SafeProjectile.IsValid() || !safePlayer.IsValid()) return;
							if (AProjectile* localProjectile = SafeProjectile.Get()) {
								localProjectile->Explode();
							}
						}, DurationTimer, false);
				}
				if (World->GetTimerManager().IsTimerActive(FCooldown)) {
					World->GetTimerManager().ClearTimer(FCooldown);
				}
				World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}
		}
	}
}
