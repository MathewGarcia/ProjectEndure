// Fill out your copyright notice in the Description page of Project Settings.

#include "FireDebuff.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Engine/DamageEvents.h"

void UFireDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
    Super::ExecuteDebuff_Implementation(Target,DamageCauser);

    TWeakObjectPtr<AActor>ActorWeakPtr = Target;

    if (APlayerCharacter* player = Cast<APlayerCharacter>(ActorWeakPtr.Get()))
    {
        if(player->CurrentDebuffs.Find(EElementTypes::Fire))
        {
            return;
        }

        player->CurrentDebuffs.Add(EElementTypes::Fire, this);

        float* ChangedVal = nullptr;
        float OriginalValue = 0.f;
        USkeletalMeshComponent* Mesh = player->GetMesh();
        TWeakObjectPtr<UMaterialInterface>safeOriginalMaterial = Mesh ? Mesh->GetMaterial(0) : nullptr;
        if(APlayerCharacterState*PCS = player->GetPlayerCharacterState())
        {
            if (float* currentVal = PCS->playerStats.Resistances.Find(EElementTypes::Physical))
            {
                OriginalValue = *currentVal;
                *currentVal = 0.f;
                ChangedVal = currentVal;
            }
        }
    
        //increase all physical damage.
        if (BurnMaterial && Mesh) {
            Mesh->SetMaterial(0, BurnMaterial);
        }
        player->GetWorldTimerManager().SetTimer(FDebuffTimerHandle, [this, ActorWeakPtr,ChangedVal,OriginalValue,safeOriginalMaterial]()
        {
            if (!ActorWeakPtr.IsValid() || !safeOriginalMaterial.IsValid()) return;

            APlayerCharacter* localPlayer = Cast<APlayerCharacter>(ActorWeakPtr.Get());
            UMaterialInterface* OriginalMaterial = safeOriginalMaterial.Get();
            if(ChangedVal)
            {
                *ChangedVal = OriginalValue;
            }

            if (localPlayer && localPlayer->GetMesh() && OriginalMaterial) {
                localPlayer->GetMesh()->SetMaterial(0, OriginalMaterial);
            }
            if (localPlayer) {
                localPlayer->CurrentDebuffs.Remove(EElementTypes::Fire);
            }
        }, DebuffTime, false);
    }
    else if (AEnemy* enemy = Cast<AEnemy>(ActorWeakPtr.Get()))
    {

        if (enemy->CurrentDebuffs.Find(EElementTypes::Fire))
        {
            return;
        }

        //increase all physical damage.

        enemy->CurrentDebuffs.Add(EElementTypes::Fire, this);


        float* ChangedVal = nullptr;
        float OriginalValue = 0.f;
        USkeletalMeshComponent* Mesh = enemy->GetMesh();
        TWeakObjectPtr<UMaterialInterface>safeOriginalMaterial = Mesh ? Mesh->GetMaterial(0) : nullptr;


        if(float*currentVal = enemy->EnemyParameters.Resistances.Find(EElementTypes::Physical))
        {
            OriginalValue = *currentVal;
            *currentVal = 0.f;
            ChangedVal = currentVal;
        }
        if (BurnMaterial && Mesh) {
            Mesh->SetMaterial(0, BurnMaterial);
        }

        enemy->GetWorldTimerManager().SetTimer(FDebuffTimerHandle, [this, ActorWeakPtr,ChangedVal,OriginalValue,safeOriginalMaterial]()
        {

            if (!ActorWeakPtr.IsValid() || !safeOriginalMaterial.IsValid()) return;

            AEnemy* localEnemy = Cast<AEnemy>(ActorWeakPtr.Get());
            UMaterialInterface* OriginalMaterial = safeOriginalMaterial.Get();

            if (ChangedVal)
            {
                *ChangedVal = OriginalValue;
            }

            if (localEnemy && localEnemy->GetMesh() && OriginalMaterial) {
                localEnemy->GetMesh()->SetMaterial(0, OriginalMaterial);
            }
            if (localEnemy) {
                localEnemy->CurrentDebuffs.Remove(EElementTypes::Fire);
            }
        }, DebuffTime, false);
    }
}

void UFireDebuff::EndBuff()
{
    Super::EndBuff();
    if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOuter()))
    {
        player->CurrentDebuffs.Remove(EElementTypes::Fire);
    }
    else if (AEnemy* enemy = Cast<AEnemy>(GetOuter()))
    {
        enemy->CurrentDebuffs.Remove(EElementTypes::Fire);
    }
}

