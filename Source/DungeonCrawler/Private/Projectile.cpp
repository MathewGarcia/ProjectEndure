// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"

#include "DungeonPieceActor.h"
#include "Enemy.h"
#include "EnvironmentalItem.h"
#include "MainGameInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AProjectile::AProjectile()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    HitBox = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere hit box"));
    HitBox->SetGenerateOverlapEvents(true);
    RootComponent = HitBox;
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->InitialSpeed = 0.0f;
    ProjectileMovementComponent->MaxSpeed = 3000.f;
    SetLifeSpan(3.f);
}

void AProjectile::OnProjectileBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || OtherActor == GetOwner() || OtherActor->IsAttachedTo(this) || (OverlappedComponent && OverlappedComponent->GetOwner() == GetOwner()))
    {
        return;
    }
    if(AEnemy*enemy = Cast<AEnemy>(OtherActor))
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy hit"));
        if(APlayerCharacter*player = Cast<APlayerCharacter>(GetOwner()))
        {
            if (AWeapon* Weapon = player->GetCurrentWeapon()) {
                float Damage = Weapon->CalculateDamage(enemy) * DamageModifier;
                switch (ProjectileType)
                {
                case EProjectileType::SingleTarget:
                    enemy->TakeDamage(Damage, FDamageEvent(), player->GetController(), player);
                    Destroy();
                    break;
                case EProjectileType::AoE:
                    Explode();
                    Destroy();
                    break;
                }
            }
        }
    }
    if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
    {
        if (AEnemy* Enemy = Cast<AEnemy>(GetOwner())) {
            float Damage = Enemy->CalculateDamage() * DamageModifier;
            player->TakeDamage(Damage,FDamageEvent(),Enemy->GetController(),Enemy);
            Destroy();
        }
    }
    if(AEnvironmentalItem* EnvironmentalItem = Cast<AEnvironmentalItem>(OtherActor))
    {
        if (EnvironmentalItem->ActorHasTag("Destructible")) {
            if (EnvironmentalItem->NiagaraSystemToPlay && EnvironmentalItem->Mesh && EnvironmentalItem->Mesh->GetStaticMesh()) {
                UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EnvironmentalItem->NiagaraSystemToPlay, EnvironmentalItem->GetActorLocation());
                if (NiagaraComponent)
                {
                    NiagaraComponent->SetVariableStaticMesh(FName("User.BrokenMesh"), EnvironmentalItem->Mesh->GetStaticMesh());
                }
            }
            EnvironmentalItem->Destroy();
        }
        Destroy();
    }
    if(ADungeonPieceActor*DungeonPieceActor = Cast<ADungeonPieceActor>(OtherActor))
    {
        Destroy();
    }
}

void AProjectile::LaunchProjectile(float Speed, const FVector& Direction)
{
    if (ProjectileMovementComponent) {
        ProjectileMovementComponent->Activate();
        ProjectileMovementComponent->Velocity = Direction*3000 *Speed;
    }
}

void AProjectile::Explode()
{
    UWorld* World = GetWorld();
    if (!World) return;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(GetOwner());
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    TArray<AActor*> HitActors;
    const float Radius = 450.f;
    const float FalloffExponent = 2.f; // sharper = higher
    if (UKismetSystemLibrary::SphereOverlapActors(World, GetActorLocation(), Radius, ObjectTypes, AEnemy::StaticClass(), ActorsToIgnore, HitActors))
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner()))
        {
            if (AWeapon* Weapon = player->GetAttackingWeapon()) {
                for (AActor* Actor : HitActors)
                {
                    if (AEnemy* Enemy = Cast<AEnemy>(Actor))
                    {
                        float Damage = Weapon->CalculateDamage(Enemy);
                        FHitResult HitResult;
                        FVector Start = Enemy->GetActorLocation();
                        FVector End = GetActorLocation();
                        if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
                        {
                            if (HitResult.GetActor() != this)
                            {
                                continue;
                            }
                        }
                        float Distance = FVector::Dist(Enemy->GetActorLocation(), GetActorLocation());
                        float t = FMath::Clamp(Distance / Radius, 0.f, 1.f);
                        float Falloff = FMath::Pow(1.f - t, FalloffExponent);
                        float FinalDamage = FMath::Lerp(Damage * 0.5f, Damage, Falloff);
                        Enemy->TakeDamage(FinalDamage, FDamageEvent(), GetOwner() ? GetOwner()->GetInstigatorController() : nullptr, GetOwner());
                    }
                }
            }
        }
    }
}

void AProjectile::Destroyed()
{
    Super::Destroyed();
    PlaySound();
}

void AProjectile::PlaySound()
{
    if(ContactSound && GetWorld())
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ContactSound, GetActorLocation(), GetActorRotation());
    }
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
    Super::BeginPlay();
    if (HitBox) {
        HitBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AProjectile::OnProjectileBeginOverlap);
    }
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

