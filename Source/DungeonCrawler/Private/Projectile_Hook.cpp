// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile_Hook.h"

#include "Ability_Hook.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"

void AProjectile_Hook::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(IsValid(HookedPlayer))
	{
		return;
	}


	if (!OwningAbility) return;

	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			if (AEnemy* Enemy = Cast<AEnemy>(GetOwner())) {
				player->TakeDamage(Enemy->Damage, FDamageEvent(), Enemy->GetController(), Enemy);
				player->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				ReturnHookProjectile();
				HookedPlayer = player;
			}
		}
		else if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
		{
			if(APlayerCharacter* owningPlayer	 = Cast<APlayerCharacter>(GetOwner()))
			{
				Enemy->TakeDamage(OwningAbility->AbilityDamage, FDamageEvent(), owningPlayer->GetController(), owningPlayer);
				Enemy->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				HookedPlayer = Enemy;
			}
		}
	}

}

void AProjectile_Hook::Tick(float DeltaSeconds)
{
 	Super::Tick(DeltaSeconds);

	if (!OwningActor) return;

	float distance = (OwningActor->GetActorLocation() - GetActorLocation()).Size();


		if (HookedPlayer && distance <= 100.f)
		{
			if (HookedPlayer->IsAttachedTo(this)) {
				HookedPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				if(AEnemy*Enemy = Cast<AEnemy>(HookedPlayer))
				{
					Enemy->SlowEnemySpeed(5.f);
				}
				Destroy();
			}
		}
		else if(GetOwner()->IsA(APlayerCharacter::StaticClass()) && distance <= 100.f)
		{
			Destroy();

		}
	
}

void AProjectile_Hook::ReturnHookProjectile()
{
	FVector HomeLocation = (OwningActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	LaunchProjectile(1.f, HomeLocation);
}
