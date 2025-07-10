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
			if(UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
			{
				safeNC = NC;
			}

			bActive = true;
			player->OnEnemyDamageTaken.Remove(FRootingProjectileDelegate);
			TWeakObjectPtr<APlayerCharacter> safePlayer = player;
			TWeakObjectPtr<UAbility_Rooting_Projectile> safeAbility = this;
			FRootingProjectileDelegate = player->OnEnemyDamageTaken.AddLambda([safeAbility,safePlayer](AEnemy* Enemy, float& Damage)
			{
					if (!safePlayer.IsValid() || !safeAbility.IsValid()) return;
					UAbility_Rooting_Projectile* localAbility = safeAbility.Get();
					APlayerCharacter* localPlayer = safePlayer.Get();
					UNiagaraComponent* localNC = localAbility->safeNC.Get();
					if (!localAbility || !localPlayer || !localNC|| !Enemy) return;

					if(UCharacterMovementComponent*CharacterMovementComponent = Enemy->GetCharacterMovement())
					{
						CharacterMovementComponent->MaxWalkSpeed = 0.f;
						TWeakObjectPtr<AEnemy> SafeEnemy = Enemy;
						localPlayer->GetWorld()->GetTimerManager().SetTimer(localAbility->FDuration, [SafeEnemy]
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
					localNC->DestroyComponent();
					localPlayer->GetWorld()->GetTimerManager().SetTimer(localAbility->FCooldown, localAbility->Cooldown, false);
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
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !bActive && !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost) && player->GetCurrentWeapon() && player->GetCurrentWeapon()->WeaponType == EWeaponType::Mage;
		}
	}
	return false;
}
