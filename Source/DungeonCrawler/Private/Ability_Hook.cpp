// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Hook.h"

#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Projectile_Hook.h"

bool UAbility_Hook::bShouldExecute_Implementation()
{
	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		return !Enemy->GetWorldTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
	}

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
				return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && !PCS->GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
			
		}

	}
	return false;
}

void UAbility_Hook::execute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		Enemy->PlayAnimMontage(AnimMontageToPlay, 1, "Hook");
	}
	else if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			player->PlayAnimMontage(AnimMontageToPlay, 1, "Hook");
		}
	}
}

void UAbility_Hook::Logic()
{
	Super::Logic();
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter())) {
		if (ProjectileToSpawn)
		{
			FActorSpawnParameters params;
			params.Owner = Enemy;
			FVector SpawnLocation = Enemy->GetActorLocation() + (Enemy->GetActorForwardVector() * 50.f);
			if(AProjectile_Hook*SpawnedHook = Enemy->GetWorld()->SpawnActor<AProjectile_Hook>(ProjectileToSpawn,SpawnLocation,Enemy->GetActorRotation(),params))
			{
				SpawnedHook->OwningActor = Enemy;
				SpawnedHook->OwningAbility = this;


				if (UMainGameInstance* MGI = Cast<UMainGameInstance>(Enemy->GetWorld()->GetGameInstance())) {
					FVector ShotDirection = (MGI->localPlayer->GetActorLocation() - SpawnedHook->GetActorLocation()).GetSafeNormal();
					SpawnedHook->LaunchProjectile(1.f, ShotDirection);

					Enemy->GetWorldTimerManager().SetTimer(FCooldown, Cooldown, false);
				}
			}
		}
	}
	else if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			if (ProjectileToSpawn)
			{
				FActorSpawnParameters params;
				params.Owner = player;
				FVector SpawnLocation = player->GetActorLocation() + (player->GetActorForwardVector() * 50.f);
				if (AProjectile_Hook* SpawnedHook = player->GetWorld()->SpawnActor<AProjectile_Hook>(ProjectileToSpawn, SpawnLocation, player->GetActorRotation(), params))
				{
					SpawnedHook->OwningActor = player;
					player->Hooks.Enqueue(SpawnedHook);
					SpawnedHook->OwningAbility = this;
					FVector ShotDirection = player->GetActorForwardVector();
					SpawnedHook->LaunchProjectile(1.f, ShotDirection);
					player->GetWorldTimerManager().SetTimer(FCooldown,Cooldown, false);
				}
			}
		}
	}

}

void UAbility_Hook::OnTriggered()
{
	Super::OnTriggered();

	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {

			if (!player->Hooks.IsEmpty())
			{
				AProjectile_Hook* CurrentHook = nullptr;
				player->Hooks.Dequeue(CurrentHook);

				if (CurrentHook)
				{
					CurrentHook->ReturnHookProjectile();
				}

			}
		}
	}
}
