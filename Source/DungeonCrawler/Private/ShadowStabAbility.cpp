// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowStabAbility.h"

#include "DebuffActor.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"

bool UShadowStabAbility::bShouldExecute_Implementation()
{
    if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
    {
        if(Enemy->GetWorldTimerManager().IsTimerActive(FCooldownTimer))
        {
            return false;
        }
        if (Enemy->MiscMontages)
        {
            for (int i = 0; i < Enemy->MiscMontages->GetNumSections(); ++i)
            {
                if (Enemy->MiscMontages->GetSectionName(i).ToString().Contains("ShadowStab"))
                {
                    if (Enemy->GetStamina() >= staminaCost) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void UShadowStabAbility::execute_Implementation()
{
    if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
    {
        if (Enemy->MiscMontages)
        {
            Enemy->PlayAnimMontage(Enemy->MiscMontages, 1.f, "ShadowStab");
        }
    }
}

void UShadowStabAbility::Logic()
{
    if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
    {
        if(UMainGameInstance*MainGI = Cast<UMainGameInstance>(Enemy->GetGameInstance()))
        {
            if (!MainGI->localPlayer) return;
            UWorld* World = Enemy->GetWorld();
            if (!World) return;
            FVector Location = MainGI->localPlayer->GetActorLocation();
            FActorSpawnParameters params;
            params.Owner = Enemy;
            if(ActorToSpawn)
            {
                if(ADebuffActor*DebuffActor = World->SpawnActor<ADebuffActor>(ActorToSpawn,Location,FRotator(0.f),params))
                {
                    World->GetTimerManager().SetTimer(FCooldownTimer, [this]() {}, CooldownTimer, false);
                }
            }
        }
    }
}
