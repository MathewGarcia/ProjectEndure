// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Rooting_Projectile.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacterState.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAbility_Rooting_Projectile::execute_Implementation()
{
	Super::execute_Implementation();
	if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (NiagaraSystem && player->GetMesh()) {
				if(UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
				{
					safeNC = NC;
				}
			}
			bActive = true;
			player->OnEnemyDamageTaken.Remove(FRootingProjectileDelegate);
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			TWeakObjectPtr<UAbility_Rooting_Projectile> safeAbility = this;
			FRootingProjectileDelegate = player->OnEnemyDamageTaken.AddLambda([safeAbility,safePlayer](AEnemy* Enemy, float& Damage)
			{
				if (!safePlayer.IsValid() || !safeAbility.IsValid() || !Enemy) return;
				UAbility_Rooting_Projectile* localAbility = safeAbility.Get();
				APlayerCharacter* localPlayer = safePlayer.Get();
				UNiagaraComponent* localNC = localAbility ? localAbility->safeNC.Get() : nullptr;
				if (!localAbility || !localPlayer || !localNC) return;
				if(UCharacterMovementComponent*CharacterMovementComponent = Enemy->GetCharacterMovement())
				{
					CharacterMovementComponent->MaxWalkSpeed = 0.f;
					TWeakObjectPtr<AEnemy> SafeEnemy = Enemy;
					UWorld* World = localPlayer->GetWorld();
					if (World && World->GetTimerManager().IsTimerActive(localAbility->FDuration)) {
						World->GetTimerManager().ClearTimer(localAbility->FDuration);
					}
					if (World)
						World->GetTimerManager().SetTimer(localAbility->FDuration, [SafeEnemy]
						{
							if (!SafeEnemy.IsValid()) return;
							AEnemy* localEnemy = SafeEnemy.Get();
							if (!localEnemy) return;
							if (UCharacterMovementComponent* CharacterMovementComponent = localEnemy->GetCharacterMovement())
							{
								CharacterMovementComponent->MaxWalkSpeed = 600.f;
							}
						}, localAbility->Duration, false);
				}
				if(localAbility->CurrentProjectileCount > localAbility->MaxProjectiles)
				{
					localPlayer->OnEnemyDamageTaken.Remove(localAbility->FRootingProjectileDelegate);
					localAbility->bActive = false;
					if (localNC)
						localNC->DestroyComponent();
					UWorld* World = localPlayer->GetWorld();
					if (World && World->GetTimerManager().IsTimerActive(localAbility->FCooldown)) {
						World->GetTimerManager().ClearTimer(localAbility->FCooldown);
					}
					if (World)
						World->GetTimerManager().SetTimer(localAbility->FCooldown, localAbility->Cooldown, false);
				}
			});
		}
	}
}

bool UAbility_Rooting_Projectile::bShouldExecute_Implementation()
{
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			UWorld* World = GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !bActive && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
		}
	}
	return false;
}
