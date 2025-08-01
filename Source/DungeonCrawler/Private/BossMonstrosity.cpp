// Fill out your copyright notice in the Description page of Project Settings.


#include "BossMonstrosity.h"
#include "AIEnemy.h"
#include "MainPlayerWidget.h"
#include "DebuffActor.h"
#include "PlayerCharacter.h"

void ABossMonstrosity::SpawnEnemies()
{
    if(EnemyToSpawn && GetWorld())
    {
        FActorSpawnParameters params;
        params.Owner = this;
        params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        for(int i = 0; i < AmountToSpawn; ++i)
        {
            FVector SpawnLocation = GetActorLocation() + FVector(FMath::FRandRange(-Radius, Radius), FMath::FRandRange(-Radius, Radius), 0.f);
            if (AEnemy* SpawnedEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyToSpawn, SpawnLocation, GetActorRotation()))
            {
                ActiveSludges.Add(SpawnedEnemy);
                if(AAIEnemy*EnemyController = Cast<AAIEnemy>(SpawnedEnemy->GetController()))
                {
                    if (EnemyController->AttackState.IsValid()) {
                        EnemyController->ChangeAIState(EnemyController->AttackState.Get());
                    }
                }
            }
        }
    }
}

void ABossMonstrosity::HandleDeath(AActor*DamageCauser)
{
    if(currentLives <= 1)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser))
        {
            if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(player->GetPlayerState()))
            {
                pcs->playerStats.UpdateEXP(EnemyParameters.Exp);
                SpawnBossBuff();
                HideBossHealthBarUI(pcs);
                Destroy();
            }
        }
        if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
        {
            if (AIEnemyController->DeadState && AIEnemyController->DeadState.IsValid()) {
                AIEnemyController->ChangeAIState(AIEnemyController->DeadState.Get());
            }
        }
    }
    else
    {
        --currentLives;
        if (GetMesh() && DeathAnimation)
        {
            GetMesh()->PlayAnimation(DeathAnimation, false);
        }
        FTimerHandle FTimer;
        if (GetWorld()) {
            GetWorldTimerManager().SetTimer(FTimer, [this]() {
                SetActorHiddenInGame(true);
                SetActorEnableCollision(false);
                if(AAIEnemy*AIEnemyController = Cast<AAIEnemy>(GetController()))
                {
                    AIEnemyController->SetAIEnabled(false);
                }
                SpawnEnemies();
            }, 1.f, false);
        }
    }
}

float ABossMonstrosity::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{
    if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
    {
        if (AIEnemyController->CurrentState == (AIEnemyController->IdleState.IsValid() ? AIEnemyController->IdleState.Get() : nullptr))
        {
            if (AIEnemyController->AttackState.IsValid()) {
                AIEnemyController->ChangeAIState(AIEnemyController->AttackState.Get());
            }
        }
    }
    if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser)) {
        HandlePlayerAttack(player, DamageAmount);
    }
    else if (ADebuffActor* DebuffActor = Cast<ADebuffActor>(DamageCauser))
    {
        HandleDebuffAttack(DebuffActor, DamageAmount);
    }
    else
    {
        Health -= DamageAmount;
        Health = FMath::Clamp(Health, 0, MaxHealth);
    }
    if (EnemyParameters.bIsBoss)
    {
        UpdateBossHealthUI();
    }
    if (Health <= 0)
    {
        if (!bDead) {
            HandleDeath(DamageCauser);
            if (APlayerCharacter* player = Cast<APlayerCharacter>(DamageCauser))
            {
                if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(player->GetPlayerState()))
                {
                    UpdatePlayerEXPUI(pcs);
                }
            }
            bDead = true;
        }
    }
    return ACharacter::Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABossMonstrosity::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(ActiveSludges.Num() > 0)
    {
        for(int i = ActiveSludges.Num() - 1; i >= 0; --i)
        {
            if(!IsValid(ActiveSludges[i]))
            {
                ActiveSludges.RemoveAt(i);
            }
        }
    }
    if(ActiveSludges.Num() <= 0 && IsHidden() && currentLives > 0)
    {
        ResetBoss();
    }
}

void ABossMonstrosity::BeginPlay()
{
    Super::BeginPlay();
    currentLives = Lives;
}

void ABossMonstrosity::ResetBoss()
{
    if (AAIEnemy* AIEnemyController = Cast<AAIEnemy>(GetController()))
    {
        SetActorHiddenInGame(false);
        SetActorEnableCollision(true);
        AIEnemyController->SetAIEnabled(true);
        if (AIEnemyController->IdleState.IsValid()) {
            AIEnemyController->ChangeAIState(AIEnemyController->IdleState.Get());
        }
        if (GetMesh()) {
            GetMesh()->SetAnimationMode(EAnimationMode::Type::AnimationBlueprint);
        }
        Health = MaxHealth;
        bDead = false;
        UpdateBossHealthUI();
    }
}

void ABossMonstrosity::ResetEnemy()
{
    Super::ResetEnemy();
    currentLives = Lives;
}
