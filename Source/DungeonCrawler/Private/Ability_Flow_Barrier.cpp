// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Flow_Barrier.h"

#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbility_Flow_Barrier::execute_Implementation()
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

bool UAbility_Flow_Barrier::bShouldExecute_Implementation()
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

void UAbility_Flow_Barrier::Logic()
{
	Super::Logic();
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = player->GetWorld();
			if (!World) return;
			TArray<AActor*>actorsToIgnore;
			TArray<AActor*> HitActors;
			actorsToIgnore.Add(player);
			TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
			if(UKismetSystemLibrary::SphereOverlapActors(World, player->GetActorLocation(), Radius, ObjectTypes,AEnemy::StaticClass(),actorsToIgnore,HitActors))
			{
				for(AActor*HitActor : HitActors)
				{
					if(AEnemy*Enemy = Cast<AEnemy>(HitActor))
					{
						if(UCharacterMovementComponent*MovementComponent = Enemy->GetCharacterMovement())
						{
							MovementComponent->MaxWalkSpeed /= 4;
						}
						if(Enemy->GetMesh())
						{
							Enemy->GetMesh()->GlobalAnimRateScale = 0.5;
						}
						TWeakObjectPtr<AEnemy> SafeEnemy = Enemy;
						FTimerHandle FLocalDurationTimer;
						UWorld* EnemyWorld = Enemy->GetWorld();
						if (!EnemyWorld) continue;
						if (EnemyWorld->GetTimerManager().IsTimerActive(FLocalDurationTimer)) {
							EnemyWorld->GetTimerManager().ClearTimer(FLocalDurationTimer);
						}
						EnemyWorld->GetTimerManager().SetTimer(FLocalDurationTimer, [SafeEnemy]
						{
							if (!SafeEnemy.IsValid()) return;
							if(AEnemy*LocalEnemy = SafeEnemy.Get())
							{
								if (UCharacterMovementComponent* MovementComponent = LocalEnemy->GetCharacterMovement())
								{
									MovementComponent->MaxWalkSpeed = 600.f;
								}
								if (LocalEnemy->GetMesh())
								{
									LocalEnemy->GetMesh()->GlobalAnimRateScale = 1.f;
								}
							}
						}, Duration, false);
					}
				}
				if (World->GetTimerManager().IsTimerActive(FCooldown)) {
					World->GetTimerManager().ClearTimer(FCooldown);
				}
				World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
			}
		}
	}

}
