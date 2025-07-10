// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_DisablePlayerHitbox.h"

#include "PlayerCharacter.h"

void UNS_DisablePlayerHitbox::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		player->ToggleHitBox(ECollisionEnabled::Type::NoCollision);
	}
}

void UNS_DisablePlayerHitbox::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		player->ToggleHitBox(ECollisionEnabled::Type::QueryAndPhysics);
	}
}
