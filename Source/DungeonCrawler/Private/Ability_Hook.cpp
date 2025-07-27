// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Hook.h"

#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Projectile_Hook.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CableComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

namespace {
    void StartPullingActor(AActor* HookedActor, AActor* OwnerActor, UWorld* World, float PullSpeed = 2000.f, float StopDistance = 150.f) {
        if (!HookedActor || !OwnerActor || !World) return;


        // Calculate effective stop distance based on capsule sizes
        float HookedRadius = 0.f;
        if (ACharacter* Char = Cast<ACharacter>(HookedActor)) {
            if (UCapsuleComponent* Capsule = Char->GetCapsuleComponent()) {
                HookedRadius = Capsule->GetScaledCapsuleRadius();
            }
        }
        float OwnerRadius = 0.f;
        if (ACharacter* Char = Cast<ACharacter>(OwnerActor)) {
            if (UCapsuleComponent* Capsule = Char->GetCapsuleComponent()) {
                OwnerRadius = Capsule->GetScaledCapsuleRadius();
            }
        }
        float EffectiveStopDistance = StopDistance + HookedRadius + OwnerRadius;

        FTimerHandle PullHandle;
        TWeakObjectPtr<AActor> WeakHooked(HookedActor);
        TWeakObjectPtr<AActor> WeakOwner(OwnerActor);
        World->GetTimerManager().SetTimer(PullHandle, [WeakHooked, WeakOwner, World, PullSpeed, EffectiveStopDistance, PullHandle]() mutable {
            if (!WeakHooked.IsValid() || !WeakOwner.IsValid()) {
                World->GetTimerManager().ClearTimer(PullHandle);
                return;
            }
            FVector Start = WeakHooked->GetActorLocation();
            FVector Target = WeakOwner->GetActorLocation();
            float Distance = (Target - Start).Size();
            float RemainingDistance = FMath::Max(0.f, Distance - EffectiveStopDistance);
            FVector Direction = (Target - Start).GetSafeNormal();
            float Step = PullSpeed * World->GetDeltaSeconds();
            float MoveDistance = FMath::Min(Step, RemainingDistance);
            FVector NewLocation = Start + Direction * MoveDistance;
            WeakHooked->SetActorLocation(NewLocation, false); // sweep for collision
            if (ACharacter* Char = Cast<ACharacter>(WeakHooked.Get())) {
                if (Char->GetCharacterMovement()) {
                    Char->GetCharacterMovement()->Velocity = FVector::ZeroVector;
                }
            }
            if ((Target - NewLocation).Size() <= EffectiveStopDistance || MoveDistance <= 0.f) {
                World->GetTimerManager().ClearTimer(PullHandle);
            }
        }, 0.01f, true);
    }
}

bool UAbility_Hook::bShouldExecute_Implementation()
{
	if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
	{
		UWorld* World = Enemy->GetWorld();
		if (!World) return false;
		return !Enemy->GetWorldTimerManager().IsTimerActive(FCooldown) && Enemy->GetStamina() >= staminaCost;
	}
	if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			UWorld* World = PCS->GetWorld();
			return PCS->LearnedAbilities.Contains(this) && PCS->EquippedAbilities.Contains(this) && World && !World->GetTimerManager().IsTimerActive(FCooldown) && player->CanPlayerDoAction(EResourceTypes::Stamina, staminaCost);
		}
	}
	return false;
}

void UAbility_Hook::execute_Implementation()
{
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
	{
		if (AnimMontageToPlay)
			Enemy->PlayAnimMontage(AnimMontageToPlay, 1, "Hook");
	}
	else if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn()))
		{
			if (AnimMontageToPlay)
				player->PlayAnimMontage(AnimMontageToPlay, 1, "Hook");
		}
	}
}

void UAbility_Hook::Logic()
{
	Super::Logic();
	if (AEnemy* Enemy = Cast<AEnemy>(GetOuter())) {
		if (ProjectileToSpawn && Enemy->GetWorld())
		{
			FActorSpawnParameters params;
			params.Owner = Enemy;
			FVector SpawnLocation = Enemy->GetActorLocation() + (Enemy->GetActorForwardVector() * 50.f);
			if(AProjectile_Hook*SpawnedHook = Enemy->GetWorld()->SpawnActor<AProjectile_Hook>(ProjectileToSpawn,SpawnLocation,Enemy->GetActorRotation(),params))
			{
				SpawnedHook->OwningActor = Enemy;
				SpawnedHook->OwningAbility = this;
				SpawnedHook->OnHookedActor.BindLambda([this, Enemy](AActor* HookedActor) {
                    StartPullingActor(HookedActor, Enemy, Enemy->GetWorld());
                });
				if (UMainGameInstance* MGI = Cast<UMainGameInstance>(Enemy->GetWorld()->GetGameInstance())) {
					if (MGI->localPlayer) {
						FVector ShotDirection = (MGI->localPlayer->GetActorLocation() - SpawnedHook->GetActorLocation()).GetSafeNormal();
						SpawnedHook->LaunchProjectile(1.f, ShotDirection);
						if (Enemy->GetWorld()) {
							if (Enemy->GetWorld()->GetTimerManager().IsTimerActive(FCooldown)) {
								Enemy->GetWorld()->GetTimerManager().ClearTimer(FCooldown);
							}
							Enemy->GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
						}
					}
				}
			}
		}
	}
	else if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(PCS->GetPawn())) {
			if (ProjectileToSpawn && player->GetWorld())
			{
				FActorSpawnParameters params;
				params.Owner = player;
				FVector SpawnLocation = player->GetActorLocation() + (player->GetActorForwardVector() * 50.f);
				if (AProjectile_Hook* SpawnedHook = player->GetWorld()->SpawnActor<AProjectile_Hook>(ProjectileToSpawn, SpawnLocation, player->GetActorRotation(), params))
				{
					SpawnedHook->OwningActor = player;
					player->Hooks.Enqueue(SpawnedHook);
					SpawnedHook->OwningAbility = this;
					SpawnedHook->OnHookedActor.BindLambda([this, player](AActor* HookedActor) {
                        StartPullingActor(HookedActor, player, player->GetWorld());
                    });
					FVector ShotDirection;
					if (AEnemy* Targeted = player->GetTargetedEnemy()) {
                        ShotDirection = (Targeted->GetActorLocation() - SpawnedHook->GetActorLocation()).GetSafeNormal();
                    } else if (player->Cam) {
                        ShotDirection = player->Cam->GetForwardVector();
                    } else {
                        ShotDirection = player->GetActorForwardVector();
                    }
					SpawnedHook->LaunchProjectile(1.f, ShotDirection);
					if (player->GetWorld()) {
						if (player->GetWorld()->GetTimerManager().IsTimerActive(FCooldown)) {
							player->GetWorld()->GetTimerManager().ClearTimer(FCooldown);
						}
					 player->GetWorld()->GetTimerManager().SetTimer(FCooldown, Cooldown, false);
					}
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

				if (IsValid(CurrentHook))
				{
					CurrentHook->bIsReturning = true;
					CurrentHook->ReturnHookProjectile();
				}

			}
		}
	}
}
