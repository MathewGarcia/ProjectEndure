// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxTrigger_Kill.h"

#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"

void ABoxTrigger_Kill::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::BeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if(OtherActor)
	{
		if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
		{
			player->TakeDamage(MAX_flt, FDamageEvent(), nullptr, nullptr);
		}
	}
}
