// Fill out your copyright notice in the Description page of Project Settings.

#include "ANS_Player_Ability_Used.h"

#include "Ability.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Projectile_Hook.h"
#include "Weapon.h"

void UANS_Player_Ability_Used::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    CalculatedTime = (TargetFrame / TotalFrames)*TotalDuration;
    bTriggered = false;
    if (!MeshComp) return;
    UObject* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner))
    {
        if (APlayerCharacterState* PCS = player->GetPlayerCharacterState()) {
            if (PCS->EquippedAbilities.IsValidIndex(player->RecentlyPressedAbility)) {
                if (UAbility* Ability = PCS->EquippedAbilities[player->RecentlyPressedAbility])
                {
                    if (AItem** Item = PCS->PlayerGear.Find(EGearType::RHand)) {
                        if (AWeapon* Weapon = Cast<AWeapon>(*Item)) {
                            player->SetAttackingWeapon(Weapon);
                        }
                    }
                    UE_LOG(LogTemp, Warning, TEXT("Triggered Logic on Ability %d: %s (%p)"), player->RecentlyPressedAbility, *Ability->GetName(), Ability);
                    Ability->Logic();
                }
            }
        }
    }
}

void UANS_Player_Ability_Used::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyEnd(MeshComp, Animation, EventReference);
    if (!MeshComp) return;
    UObject* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner))
    {
        player->SetPlayerState(PlayerStates::NONE);
        player->RecentlyPressedAbility = -1;
    }
}

void UANS_Player_Ability_Used::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
    if (bTriggered) return;
    if (!MeshComp) return;
    UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
    if (!AnimInstance) return;
    float CurrentTime = AnimInstance->Montage_GetPosition(AnimInstance->GetCurrentActiveMontage());
    if (CurrentTime >= CalculatedTime) {
        UObject* Owner = MeshComp->GetOwner();
        if (!Owner) return;
        if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner))
        {
            bTriggered = true;
            if (APlayerCharacterState* PCS = player->GetPlayerCharacterState()) {
                if (PCS->EquippedAbilities.IsValidIndex(player->RecentlyPressedAbility)) {
                    if (UAbility* Ability = PCS->EquippedAbilities[player->RecentlyPressedAbility])
                    {
                        Ability->OnTriggered();
                    }
                }
            }
        }
    }
}
