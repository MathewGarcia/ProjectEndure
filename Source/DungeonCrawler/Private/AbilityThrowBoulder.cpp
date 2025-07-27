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
		if (Enemy->MiscMontages)
			Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "ThrowBoulder");
	}
}

bool UAbilityThrowBoulder::bShouldExecute_Implementation()
{
	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		UWorld* World = GetWorld();
		if(Enemy->EnemyParameters.OffensiveAbilities.Contains(this))
		{
			return World && !World->GetTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
		}
	}
	return false;
}

void UAbilityThrowBoulder::Logic()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		UWorld* World = Enemy->GetWorld();
		if (!World || !ProjectileToSpawn) return;
		FActorSpawnParameters params;
		params.Owner = Enemy;
		if(AProjectile* proj = World->SpawnActor<AProjectile>(ProjectileToSpawn,Enemy->GetActorLocation(),FRotator(0.f),params))
		{
			if (Enemy->GetMesh() && Enemy->GetMesh()->DoesSocketExist("hand_r"))
			{
				proj->AttachToComponent(Enemy->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_r");
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Socket doesnt exist or mesh is null"));
			}
			if (Enemy->MiscMontages)
			{
				int32 SectionIndex = Enemy->MiscMontages->GetSectionIndex(FName("ThrowBoulder"));
				if(SectionIndex != INDEX_NONE && Enemy->GetMesh() && Enemy->GetMesh()->GetAnimInstance())
				{
					UAnimInstance* AnimInstance = Enemy->GetMesh()->GetAnimInstance();
					UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
					if (CurrentMontage)
					{
						float CurrentPosition = AnimInstance->Montage_GetPosition(Enemy->MiscMontages);
						float SectionLength = CurrentMontage->GetSectionLength(SectionIndex);
						float LocalTime = CurrentPosition - SectionLength;
						float TimeUntilThrow = (0.4f * SectionLength) - LocalTime;
						if (TimeUntilThrow < 0.f)
						{
							TimeUntilThrow = 0.f;
						}
						if (World->GetTimerManager().IsTimerActive(ThrowTimer)) {
							World->GetTimerManager().ClearTimer(ThrowTimer);
						}
						World->GetTimerManager().SetTimer(ThrowTimer, [this, Enemy, proj]()
							{
								if (proj)
								{
									proj->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
									if (proj->ProjectileMovementComponent)
									{
										proj->ProjectileMovementComponent->Activate();
									}
									proj->LaunchProjectile(1.0, Enemy->GetActorForwardVector());
									if (proj->HitBox)
										proj->HitBox->SetCollisionProfileName("OverlapAllDynamic");
								}
							}, TimeUntilThrow, false);
						if (World->GetTimerManager().IsTimerActive(FCooldown)) {
							World->GetTimerManager().ClearTimer(FCooldown);
						}
						World->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
					}
				}
			}
		}
	}
}
