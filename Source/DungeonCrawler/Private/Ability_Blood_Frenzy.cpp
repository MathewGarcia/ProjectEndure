// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability_Blood_Frenzy.h"

#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UAbility_Blood_Frenzy::PlayWidget(APlayerCharacter* Player)
{
    if (!Player || !Player->CurrentLivesWidget || !WidgetToShow) return;
    Player->CurrentLivesWidget->SetWidgetClass(WidgetToShow);
    Player->CurrentLivesWidget->SetVisibility(true);
    FTimerHandle LocalTimer;
    UWorld* World = GetWorld();
    if (!World) return;
    World->GetTimerManager().SetTimer(LocalTimer, [Player]
        {
            if (Player && Player->CurrentLivesWidget) {
                Player->ResetCurrentLivesWidget();
                Player->CurrentLivesWidget->SetVisibility(false);
            }
        }, 1.f, false);
}

void UAbility_Blood_Frenzy::execute_Implementation()
{
    Super::execute_Implementation();
    if (APlayerCharacterState* PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
        {
            if (UCharacterMovementComponent* CharacterMovementComponent = player->GetCharacterMovement())
            {
                PlaySound();
                PlayNiagaraSystem(player);
                PlayWidget(player);
                PCS->DamageMultiplier *= 3.f;
                CharacterMovementComponent->MaxWalkSpeed += CharacterMovementComponent->MaxWalkSpeed * 0.8f;
                KillCount = 0;
                FTimerHandle FStartTimer;
                UWorld* World = GetWorld();
                if (!World) return;
                World->GetTimerManager().SetTimer(FStartTimer, [this,PCS,CharacterMovementComponent]
                {
                    if (PCS && CharacterMovementComponent) {
                        PCS->DamageMultiplier /= 3.f;
                        CharacterMovementComponent->MaxWalkSpeed = 600;
                    }
                }, Duration, false);
            }
        }
    }

}

bool UAbility_Blood_Frenzy::bShouldExecute_Implementation()
{
    if(APlayerCharacterState*PCS = Cast<APlayerCharacterState>(GetOuter()))
    {
        return PCS->LearnedAbilities.Contains(this) && KillCount >= KillsNeeded;
    }


    return false;
}

void UAbility_Blood_Frenzy::PlaySound()
{
    if (SoundToPlay) {
        UWorld* World = GetWorld();
        if (World)
            UGameplayStatics::PlaySound2D(World, SoundToPlay);
    }
}

void UAbility_Blood_Frenzy::PlayNiagaraSystem(APlayerCharacter* player)
{
    if (!player || !NiagaraSystem || !player->GetMesh()) return;
    UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, player->GetMesh(), "root", FVector::ZeroVector,FRotator::ZeroRotator,EAttachLocation::Type::SnapToTargetIncludingScale,true);
}
