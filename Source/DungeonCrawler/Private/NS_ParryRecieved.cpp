// Fill out your copyright notice in the Description page of Project Settings.


#include "NS_ParryRecieved.h"

#include "Enemy.h"

void UNS_ParryRecieved::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	if(AEnemy*Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		Enemy->EnemyState = EEnemyStates::parried;
	}
}

void UNS_ParryRecieved::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		Enemy->EnemyState = EEnemyStates::none;
	}
}
