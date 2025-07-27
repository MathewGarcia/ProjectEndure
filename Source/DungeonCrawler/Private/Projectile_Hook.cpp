// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile_Hook.h"
#include "Ability_Hook.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "Delegates/Delegate.h"
#include "CableComponent.h"

void AProjectile_Hook::BeginPlay()
{
    Super::BeginPlay();
    if (OwningActor)
    {
        USkeletalMeshComponent* OwnerMesh = OwningActor->FindComponentByClass<USkeletalMeshComponent>();
        // Attach cable to the projectile itself
        Cable = NewObject<UCableComponent>(this);
        Cable->NumSegments = 40;
        Cable->CableWidth = 5.f;
        Cable->bAttachStart = true;
        Cable->bAttachEnd = true;
        Cable->RegisterComponent();
        Cable->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale); // Attach to projectile
        // Attach the END of the cable to the owner
        Cable->SetAttachEndToComponent(OwnerMesh ? OwnerMesh : OwningActor->GetRootComponent());
    }
}

void AProjectile_Hook::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(IsValid(HookedPlayer))
    {
        return;
    }
    if (!OwningAbility) return;
    if (OtherActor && OtherActor != this && OtherActor != GetOwner())
    {
        if (Cable)
        {
            Cable->DestroyComponent();
            Cable = nullptr;
        }
        if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
        {
            if (AEnemy* Enemy = Cast<AEnemy>(GetOwner())) {
                player->TakeDamage(Enemy->Damage, FDamageEvent(), Enemy->GetController(), Enemy);
                HookedPlayer = player;
                if (OnHookedActor.IsBound()) {
                    OnHookedActor.Execute(player);
                }
                Destroy();
            }
        }
        else if (AEnemy* Enemy = Cast<AEnemy>(OtherActor))
        {
            if(APlayerCharacter* owningPlayer = Cast<APlayerCharacter>(GetOwner()))
            {
                Enemy->TakeDamage(OwningAbility->AbilityDamage, FDamageEvent(), owningPlayer->GetController(), owningPlayer);
                HookedPlayer = Enemy;
                if (OnHookedActor.IsBound()) {
                    OnHookedActor.Execute(Enemy);
                }
                Destroy();
            }
        }
    }
}

void AProjectile_Hook::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    // No more SetActorLocation logic here; handled by ability
    if (bIsReturning && OwningActor && !HookedPlayer)
    {
        float Distance = FVector::Dist(GetActorLocation(), OwningActor->GetActorLocation());
        if (Distance < 150.f) // threshold can be adjusted
        {
            Destroy();
        }
    }
}

void AProjectile_Hook::ReturnHookProjectile()
{
    if (!OwningActor) return;
    bIsReturning = true;
    FVector HomeLocation = (OwningActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
    LaunchProjectile(1.f, HomeLocation);
}
