// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayDodgeSound.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "MainGameInstance.h"

void UPlayDodgeSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;
	UObject* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	UWorld* World = nullptr;
	if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner))
	{
		World = player->GetWorld();
		if (World)
		{
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
			{
				MGI->PlayDodgeSound(player);
			}
		}
	}
	else if (AEnemy* Enemy = Cast<AEnemy>(Owner))
	{
		World = Enemy->GetWorld();
		if (World)
		{
			if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
			{
				MGI->PlayDodgeSound(Enemy);
			}
		}
	}
}
