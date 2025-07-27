// Fill out your copyright notice in the Description page of Project Settings.

#include "AoE_Heal.h"

#include "AoEHealActor.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAoE_Heal::execute_Implementation()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        if (MontageToPlay)
        {
            player->PlayAnimMontage(MontageToPlay, 1, "AoEHeal");
        }
    }
}

bool UAoE_Heal::bShouldExecute_Implementation()
{
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        return !player->GetWorldTimerManager().IsTimerActive(FCooldownTimer) && player->CanPlayerDoAction(EResourceTypes::Mana, 0);
    }
    return false;
}

void UAoE_Heal::Logic()
{
    if(ActorToSpawn)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
        {
            UWorld* World = player->GetWorld();
            if (!World) return;
            if(AAoEHealActor* AoEHealActor = World->SpawnActorDeferred<AAoEHealActor>(ActorToSpawn, player->GetActorTransform(), nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn))
            {
                AoEHealActor->OwningObject = this;
                UGameplayStatics::FinishSpawningActor(AoEHealActor, player->GetActorTransform());
                FTimerHandle FLifeCycle;
                player->GetWorldTimerManager().SetTimer(FLifeCycle, [AoEHealActor]()
                    {
                        if (AoEHealActor)
                        {
                            AoEHealActor->GetWorldTimerManager().ClearAllTimersForObject(AoEHealActor);
                            AoEHealActor->Destroy();
                        }
                    }, LifeCycle, false);
                player->GetWorldTimerManager().SetTimer(FCooldownTimer, [this]() {}, CooldownTime, false);
            }
        }
    }
}
