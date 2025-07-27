// Fill out your copyright notice in the Description page of Project Settings.

#include "NS_AOE_Stun.h"

#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/KismetSystemLibrary.h"

void UNS_AOE_Stun::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                          const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp) return;

    Super::Notify(MeshComp, Animation, EventReference);

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
    TArray<AActor*> HitActors;
    if (AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner())) {
        TArray<AActor*> ActorsToIgnore;
        ActorsToIgnore.Add(Enemy);
        UWorld* World = Enemy->GetWorld();
        if (!World) return;
        //DrawDebugSphere(World, Enemy->GetActorLocation(), 800.f, 15, FColor::Green,true);
        if(UKismetSystemLibrary::SphereOverlapActors(World, Enemy->GetActorLocation(), 800.f, ObjectTypes, APlayerCharacter::StaticClass(), ActorsToIgnore, HitActors))
        {
            for(AActor*Actor : HitActors)
            {
                if(APlayerCharacter*player = Cast<APlayerCharacter>(Actor))
                {
                    player->TakeDamage(30, FDamageEvent(), Enemy->GetInstigatorController(), Enemy);
                    player->PlayerStunned();
                }
            }
        }
    }
}
