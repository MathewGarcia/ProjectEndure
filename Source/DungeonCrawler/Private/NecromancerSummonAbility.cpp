// Fill out your copyright notice in the Description page of Project Settings.

#include "NecromancerSummonAbility.h"

#include "AIEnemy.h"
#include "Enemy.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"

bool UNecromancerSummonAbility::bShouldExecute_Implementation()
{
    if(AEnemy*Enemy = Cast<AEnemy>(GetOuter()))
    {
        if (Enemy->GetWorldTimerManager().IsTimerActive(FSummonCoolDownTimer))
        {
            return false;
        }
        Enemies.RemoveAll([](AEnemy* Enemy) {return !IsValid(Enemy); });
        if (Enemy->MiscMontages)
        {
            for(int i = 0; i < Enemy->MiscMontages->GetNumSections(); ++i)
            {
                if (Enemy->MiscMontages->GetSectionName(i).ToString().Contains("Summon"))
                {
                    //check the amount of enemies currently alive.
                    return Enemy->GetStamina() >= staminaCost && Enemies.Num() < amountToSpawn;
                }
            }
        }
    }
    return false;
}

void UNecromancerSummonAbility::execute_Implementation()
{
    if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
    {
        if (Enemy->MiscMontages)
        {
            Enemy->PlayAnimMontage(Enemy->MiscMontages, 1, "Summon");
        }
    }
}

void UNecromancerSummonAbility::Logic()
{
    if (AEnemy* Enemy = Cast<AEnemy>(GetOuter()))
    {
        if(UMainGameInstance*MainGI = Cast<UMainGameInstance>(Enemy->GetGameInstance()))
        {
            int numberToSpawn = amountToSpawn - Enemies.Num();
            for(int i = 0; i < numberToSpawn; ++i)
            {
                TSubclassOf<AEnemy> ChosenEnemy = EnemyToSpawn();
                if (!ChosenEnemy) continue;
                if (!MainGI->localPlayer) continue;
                FVector Location = MainGI->localPlayer->GetActorLocation() + FVector(FMath::FRandRange(-Distance, Distance), FMath::FRandRange(-Distance, Distance),0.f);
                FActorSpawnParameters params;
                params.Owner = Enemy;
                UWorld* World = Enemy->GetWorld();
                if (World)
                {
                    if (AEnemy* SpawnedEnemy = World->SpawnActor<AEnemy>(ChosenEnemy,Location,FRotator()))
                    {
                        Enemies.Add(SpawnedEnemy);
                        if(AAIEnemy*AIEnemy = Cast<AAIEnemy>(SpawnedEnemy->GetController()))
                        {
                            AIEnemy->ChangeAIState(AIEnemy->AttackState.Get());
                            AIEnemy->Player = MainGI->localPlayer;
                        }
                    }
                }
            }
            UWorld* World = Enemy->GetWorld();
            if (World)
            {
                Enemy->GetWorldTimerManager().SetTimer(FSummonCoolDownTimer, [this]() {}, 60.f, false);
            }
        }
    }
}

TSubclassOf<AEnemy> UNecromancerSummonAbility::EnemyToSpawn()
{
    float TotalWeight = 0.f;
    for(const auto&pair : EnemySpawnMap)
    {
        TotalWeight += pair.Value;
    }
    if (TotalWeight <= 0) return nullptr;
    float Roll = FMath::FRandRange(0.f, TotalWeight);
    for(const auto&pair : EnemySpawnMap)
    {
        Roll -= pair.Value;
        if(Roll <= 0.f)
        {
            return pair.Key;
        }
    }
    return nullptr;
}
