// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_EnemyAttack.h"

#include "Enemy.h"
#include "PlayerCharacter.h"

void UNS_EnemyAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	if (AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{

		if (Enemy->EnemyState == EEnemyStates::hit || Enemy->EnemyState == EEnemyStates::backstabbed || Enemy->EnemyState == EEnemyStates::parried || Enemy->EnemyState == EEnemyStates::idleAttack) return;

		Enemy->FacePlayer();
		 if (AbilityUsed)
		{
			Enemy->AbilityLogic(AbilityType, AbilityPos);
		}
		else if (!bProjectile) {
			 Enemy->HitEnemies.Empty();
			for (FName CollisionName : CollisionNames) {
				if (UPrimitiveComponent* AttackCollision = Enemy->GetCollisionByName(CollisionName))
				{
					AttackCollision->SetCollisionProfileName(TEXT("AttackCollision"));
					if (bShouldStun) {
						TArray<AActor*>OverlappingActors;
						AttackCollision->GetOverlappingActors(OverlappingActors, APlayerCharacter::StaticClass());
						for (AActor* Actor : OverlappingActors)
						{
							if (APlayerCharacter* player = Cast<APlayerCharacter>(Actor))
							{
								player->PlayerStunned();
							}
						}
					}
				}
			}
		}
		else
		{
			Enemy->ShootProjectile();
		}
	}
}

void UNS_EnemyAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if(AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		for (FName CollisionName : CollisionNames) {
			if (UPrimitiveComponent* AttackCollision = Enemy->GetCollisionByName(CollisionName))
			{
				AttackCollision->SetCollisionProfileName(TEXT("ActualNoCollision"));

			}
		}

		if (Enemy->EnemyState == EEnemyStates::idleAttack) return;

		Enemy->EnemyState = EEnemyStates::none;
	}
}
