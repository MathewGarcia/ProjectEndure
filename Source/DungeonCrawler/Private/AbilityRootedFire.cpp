// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityRootedFire.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbilityRootedFire::execute_Implementation()
{
    if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
    {
        if (Enemy->MiscMontages)
            Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "RootedFire");
    }
}

bool UAbilityRootedFire::bShouldExecute_Implementation()
{

    if(GetOuter()->IsA(APlayerCharacter::StaticClass()))
    {
        // Player logic not implemented
    }
    else if (AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
    {
        UWorld* World = Enemy->GetWorld();
        if (!World) return false;
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
        TArray<AActor*> ActorsToIgnore;
        ActorsToIgnore.Add(Enemy);
        TArray<AActor*> ActorsHit;
        if (UKismetSystemLibrary::SphereOverlapActors(World, Enemy->GetActorLocation(), SphereRadius, ObjectTypes, APlayerCharacter::StaticClass(), ActorsToIgnore, ActorsHit)) {
            if (Enemy->GetStamina() >= staminaCost && Enemy->EnemyParameters.OffensiveAbilities.Contains(this) && Enemy->MiscMontages)
            {
                for(int i = 0; i < Enemy->MiscMontages->GetNumSections(); ++i)
                {
                    if(Enemy->MiscMontages->GetSectionName(i).ToString().Contains("RootedFire"))
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void UAbilityRootedFire::Logic()
{
    AEnemy* Enemy = Cast<AEnemy>(GetOuter());
    UWorld* World = Enemy ? Enemy->GetWorld() : nullptr;
    if (!Enemy || !World || !ProjectileToSpawn) return;
    for(int i = 0; i < amountToSpawn; ++i)
    {
        FActorSpawnParameters params;
        params.Owner = Enemy;
        params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        float Rand = FMath::RandRange(0.0f, 1.0f);
        float Radius = SphereRadius * FMath::Sqrt(Rand);
        float Angle = FMath::RandRange(0.0f, 2.0f * PI);
        float OffsetX = Radius * FMath::Cos(Angle);
        float OffsetY = Radius * FMath::Sin(Angle);
        FVector Location = Enemy->GetActorLocation() + FVector(OffsetX, OffsetY, 0.0f);
        if(AProjectile*RootedFireProjectile = World->SpawnActor<AProjectile>(ProjectileToSpawn,Location, FRotator(0.f), params))
        {
            if(RootedFireProjectile->HitBox)
            {
                RootedFireProjectile->HitBox->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
            }
            Projectiles.AddUnique(RootedFireProjectile);
        }
    }
    if(Enemy)
    {
        FTimerHandle TimerHandle;
        if (World->GetTimerManager().IsTimerActive(TimerHandle)) {
            World->GetTimerManager().ClearTimer(TimerHandle);
        }
        World->GetTimerManager().SetTimer(TimerHandle, [this]() {
            for(AProjectile*proj : Projectiles )
            {
                if (proj) {
                    FVector Start = proj->GetActorLocation();
                    FVector End = Start + FVector::UpVector * 700;
                    if (UProjectileMovementComponent* PMC = Cast<UProjectileMovementComponent>(proj->GetComponentByClass(UProjectileMovementComponent::StaticClass())))
                    {
                        if (proj->HitBox)
                        {
                            proj->HitBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
                        }
                        PMC->Activate();
                    }
                    proj->LaunchProjectile(1.f, End);
                }
            }
        }, timeToLaunch, false);
    }
}
