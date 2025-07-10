// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityThrowBoulder.h"

#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UAbilityThrowBoulder::execute_Implementation()
{
	Super::execute_Implementation();

	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "ThrowBoulder");
	}
}

bool UAbilityThrowBoulder::bShouldExecute_Implementation()
{
	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		if(Enemy->EnemyParameters.OffensiveAbilities.Contains(this))
		{
			return !GetWorld()->GetTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
		}
	}
	return false;
}

void UAbilityThrowBoulder::Logic()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if(ProjectileToSpawn)
		{
			FActorSpawnParameters params;
			params.Owner = Enemy;
			if(AProjectile* proj = Enemy->GetWorld()->SpawnActor<AProjectile>(ProjectileToSpawn,Enemy->GetActorLocation(),FRotator(0.f),params))
			{

				if(!Enemy->GetMesh()->DoesSocketExist("hand_r"))
				{
					UE_LOG(LogTemp, Warning, TEXT("Socket doesnt exist"));
				}
				else {
					proj->AttachToComponent(Enemy->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_r");
				}
				int32 SectionIndex = Enemy->MiscMontages->GetSectionIndex(FName("ThrowBoulder"));
				
				if(SectionIndex != INDEX_NONE)
				{
					float CurrentPosition = Enemy->GetMesh()->GetAnimInstance()->Montage_GetPosition(Enemy->MiscMontages);

					float SectionLength = Enemy->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage()->GetSectionLength(SectionIndex);

					float LocalTime = CurrentPosition - SectionLength;
					float TimeUntilThrow = (0.4f * SectionLength) - LocalTime;
					// Don’t schedule negative times
					if (TimeUntilThrow < 0.f)
					{
						TimeUntilThrow = 0.f;
					}

					Enemy->GetWorldTimerManager().SetTimer(ThrowTimer, [this, Enemy, proj]()
						{
						
							proj->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

							if (proj->ProjectileMovementComponent)
							{
								proj->ProjectileMovementComponent->Activate();
							}
							// Fire projectile
							proj->LaunchProjectile(1.0, Enemy->GetActorForwardVector());
							proj->HitBox->SetCollisionProfileName("OverlapAllDynamic");

						}, TimeUntilThrow, false);

					GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
				}

			}
		}
	}
}
