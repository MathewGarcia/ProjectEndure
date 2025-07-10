// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowDebuff.h"

#include "DebuffActor.h"
#include "Engine/DamageEvents.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.h"

void UShadowDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
	Super::ExecuteDebuff_Implementation(Target, DamageCauser);

	FVector DirectionToPlayer = Target->GetActorLocation() - DamageCauser->GetActorLocation();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	params.Owner = DamageCauser;

	FVector Location = DamageCauser->GetActorLocation() + DamageCauser->GetActorForwardVector() * 10.f;
	ADebuffActor* DebuffActor = DamageCauser->GetWorld()->SpawnActor<ADebuffActor>(ActorToSpawn,Location, DirectionToPlayer.Rotation(), params);
	if (DebuffActor) {
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(DamageCauser->GetWorld(), NiagaraSystem, DebuffActor->GetActorLocation());
	}

	Target->GetWorldTimerManager().SetTimer(FDebuffTimerHandle, [this, DebuffActor]()
		{
			if (DebuffActor)
			{
				DebuffActor->Destroy();
			}
		}, DebuffTime, false);
}
