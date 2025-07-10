// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify_ResetAttack.h"

#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UNotify_ResetAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (!bIsDodge) {
			player->IncrementAttackCount();
		}
		player->SetPlayerState(PlayerStates::NONE);
		player->SetAttackingWeapon(nullptr);
	}

}
