// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayGroundSound.h"

#include "MainGameInstance.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

void UPlayGroundSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	if(APlayerCharacter*player = Cast<APlayerCharacter>(MeshComp->GetOwner()))
	{
		player->PlaySoundBasedOnSurface();
	}
	else if (AEnemy*Enemy = Cast<AEnemy>(MeshComp->GetOwner()))
	{
		if(Enemy->EnemyType == EEnemyType::imp)
		{
			if(UMainGameInstance*MGI = Cast<UMainGameInstance>(Enemy->GetWorld()->GetGameInstance()))
			{
				USoundBase* Sound = MGI->WingSounds[FMath::RandRange(0, MGI->WingSounds.Num() - 1)];

				if (Sound)
				{

					UGameplayStatics::PlaySoundAtLocation(Enemy->GetWorld(), Sound, Enemy->GetActorLocation(), FRotator::ZeroRotator,1,1,0,MGI->Attenuation);

				}
			}
		}
		Enemy->PlaySoundBasedOnSurface();
	}

}
