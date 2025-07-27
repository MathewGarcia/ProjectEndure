// Fill out your copyright notice in the Description page of Project Settings.

#include "Drink_Interactable_Actor.h"
#include "MainGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"

void ADrink_Interactable_Actor::Interact_Implementation()
{
    Super::Interact_Implementation();
    UWorld* World = GetWorld();
    if (!World) return;
    if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
    {
        if(APlayerCharacter*player = MGI->localPlayer)
        {
            player->SetHealAbility(HealAbility);
            if (NiagaraSystemToPlay && player->GetMesh()) {
                UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystemToPlay, player->GetMesh(),"spine_02",FVector::ZeroVector, FRotator::ZeroRotator,EAttachLocation::Type::SnapToTargetIncludingScale,true);
            }
        }
    }
}
