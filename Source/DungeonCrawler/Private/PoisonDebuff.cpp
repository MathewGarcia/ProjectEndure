// Fill out your copyright notice in the Description page of Project Settings.

#include "PoisonDebuff.h"

#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/DamageEvents.h"
#include "PlayerCharacter.h"

void UPoisonDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
    Super::ExecuteDebuff_Implementation(Target,DamageCauser);
    UWorld* World = GetWorld();
    if (!World) return;
    if (APlayerCharacter* player = Cast<APlayerCharacter>(Target))
    {
        if(player->CurrentDebuffs.Contains(EElementTypes::Poison))
        {
            UElementDebuff* Existing = player->CurrentDebuffs[EElementTypes::Poison];
            //if we already have an existing poison debuff, return
            if(IsValid(Existing))
            {
                return;
            }

            //if existing is null, remove it. i.e, in the map its actuall nullptr.
            player->CurrentDebuffs.Remove(EElementTypes::Poison);

        }

        player->CurrentDebuffs.Add(EElementTypes::Poison, this);

        UNiagaraComponent* NiagaraComponent = nullptr;
        if (NiagaraSystem && player->GetMesh()) {
            NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "ChestGear", FVector(0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, false, true);
            if (!NiagaraComponent)
            {
                UE_LOG(LogTemp, Warning, TEXT("Niagara Component failed"));
            }
        }

        TWeakObjectPtr<APlayerCharacter> safePlayer = player;
        TWeakObjectPtr<UPoisonDebuff> SafeDebuff = this;
        World->GetTimerManager().SetTimer(FInnerTimerHandle, [safePlayer]()
            {
                if (!safePlayer.IsValid()) return;

                APlayerCharacter* localPlayer = safePlayer.Get();

                if (!localPlayer) return;

                localPlayer->TakeDamage(3.f, FDamageEvent(), nullptr, nullptr);

            }, TickRate, true);

        World->GetTimerManager().SetTimer(FDebuffTimerHandle, [SafeDebuff,safePlayer, NiagaraComponent]()
            {

                if (!safePlayer.IsValid() || !SafeDebuff.IsValid()) return;

                APlayerCharacter* localPlayer = safePlayer.Get();

                if (!IsValid(localPlayer)) return;

                if (UPoisonDebuff* LocalPoisonDebuff = SafeDebuff.Get()) {
                    LocalPoisonDebuff->EndBuff();
                    if (NiagaraComponent) {
                        NiagaraComponent->DestroyComponent();
                    }
                }
            }, DebuffTime, false);
    }
    else if (AEnemy* enemy = Cast<AEnemy>(Target))
    {
        if (enemy->CurrentDebuffs.Contains(EElementTypes::Poison))
        {
            if (UElementDebuff** Existing = enemy->CurrentDebuffs.Find(EElementTypes::Poison)) {
                if (IsValid(*Existing))
                {
                    return;
                }
            }
            enemy->CurrentDebuffs.Remove(EElementTypes::Poison);

        }

        enemy->CurrentDebuffs.Add(EElementTypes::Poison, this);

        UNiagaraComponent* NiagaraComponent = nullptr;
        if (NiagaraSystem && enemy->GetMesh()) {
            NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, enemy->GetMesh(), "spine_01", FVector(0.f), FRotator(0.f), EAttachLocation::Type::SnapToTarget, false, true);
        }

        TWeakObjectPtr<AEnemy> safeEnemy = enemy;
        TWeakObjectPtr<UPoisonDebuff> SafeDebuff = this;
        World->GetTimerManager().SetTimer(FInnerTimerHandle, [safeEnemy]()
            {
                if (!safeEnemy.IsValid()) return;

                AEnemy* localEnemy = safeEnemy.Get();

                if (!localEnemy) return;

                localEnemy->TakeDamage(3.f, FDamageEvent(), nullptr, nullptr);

            }, TickRate, true);

        World->GetTimerManager().SetTimer(FDebuffTimerHandle, [SafeDebuff, safeEnemy, NiagaraComponent]()
            {

                if (!safeEnemy.IsValid() || !SafeDebuff.IsValid()) return;

                UPoisonDebuff* LocalDebuff = SafeDebuff.Get();
                AEnemy* localEnemy = safeEnemy.Get();

                if (!IsValid(localEnemy) || !IsValid(LocalDebuff)) return;

                localEnemy->CurrentDebuffs.Remove(EElementTypes::Poison);

                if (LocalDebuff->GetWorld() && LocalDebuff->GetWorld()->GetTimerManager().IsTimerActive(LocalDebuff->FInnerTimerHandle))
                {
                    LocalDebuff->GetWorld()->GetTimerManager().ClearTimer(LocalDebuff->FInnerTimerHandle);
                }
                if (NiagaraComponent) {
                    NiagaraComponent->DestroyComponent();
                }

            }, DebuffTime, false);
    }
}

void UPoisonDebuff::EndBuff()
{
    Super::EndBuff();
    UWorld* World = GetWorld();
    if (World) {
        World->GetTimerManager().ClearTimer(FInnerTimerHandle);
        World->GetTimerManager().ClearTimer(FDebuffTimerHandle);
    }
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        player->CurrentDebuffs.Remove(EElementTypes::Poison);
    }
    else if (AEnemy* enemy = Cast<AEnemy>(GetOuter()))
    {
        enemy->CurrentDebuffs.Remove(EElementTypes::Poison);
    }
}
