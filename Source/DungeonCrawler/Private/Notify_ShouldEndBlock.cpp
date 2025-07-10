// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify_ShouldEndBlock.h"

#include "Enemy.h"

void UNotify_ShouldEndBlock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if(AEnemy*Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		if(!Enemy->ShouldBlock())
		{
			Enemy->EndBlock();
		}
	}
}
