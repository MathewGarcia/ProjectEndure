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
	UObject* Owner = MeshComp->GetOwner();
	if (!Owner) return;
	if(APlayerCharacter*player = Cast<APlayerCharacter>(Owner))
	{
		player->PlaySoundBasedOnSurface();
	}
	else if (AEnemy*Enemy = Cast<AEnemy>(Owner))
	{
		if(Enemy->EnemyType == EEnemyType::imp)
		{
			UWorld* World = Enemy->GetWorld();
			if (!World) return;
			if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
			{
				if (MGI->WingSounds.Num() > 0) {
					int32 Index = FMath::RandRange(0, MGI->WingSounds.Num() - 1);
					if (MGI->WingSounds.IsValidIndex(Index)) {
						USoundBase* Sound = MGI->WingSounds[Index];
						if (Sound)
						{

							UGameplayStatics::PlaySoundAtLocation(World, Sound, Enemy->GetActorLocation(), FRotator::ZeroRotator,1,1,0,MGI->Attenuation);

						}
					}
				}
			}
		}
		Enemy->PlaySoundBasedOnSurface();
	}
}
