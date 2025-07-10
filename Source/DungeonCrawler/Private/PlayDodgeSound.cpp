// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayDodgeSound.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "MainGameInstance.h"

void UPlayDodgeSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	if (APlayerCharacter* player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(player->GetWorld()->GetGameInstance()))
		{
			MGI->PlayDodgeSound(player);
		}
	}
	else if (AEnemy* Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(Enemy->GetWorld()->GetGameInstance()))
			{
				MGI->PlayDodgeSound(Enemy);
			}
		
	}
}
