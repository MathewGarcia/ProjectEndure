// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxTrigger_Reset.h"

#include "PlayerCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ABoxTrigger_Reset::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);



	if (OtherActor)
	{
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
		{
			if(AActor*PlayerStart = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerStart::StaticClass()))
			{
				player->SetActorLocation(PlayerStart->GetActorLocation());

			}
		}
	}

}
