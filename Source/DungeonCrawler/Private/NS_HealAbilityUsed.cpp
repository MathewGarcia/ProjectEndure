// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_HealAbilityUsed.h"

#include "Ability.h"
#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UNS_HealAbilityUsed::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		player->EquippedHealAbility->Logic();
	}
}

void UNS_HealAbilityUsed::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);


	if (!MeshComp) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		player->SetPlayerState(PlayerStates::NONE);
	}
}
