// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_ResetAttackCount.h"

#include "PlayerCharacter.h"

void UAN_ResetAttackCount::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;
	UObject* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner))
	{
		player->ResetAttackCount();
	}
}
