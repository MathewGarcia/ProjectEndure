// Fill out your copyright notice in the Description page of Project Settings.


#include "ShadowDebuff.h"
#include "DebuffActor.h"
#include "NiagaraFunctionLibrary.h"

void UShadowDebuff::ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser)
{
	Super::ExecuteDebuff_Implementation(Target, DamageCauser);
	if (!Target || !DamageCauser) return;
	UWorld* World = Target->GetWorld();
	if (!World) return;
	FVector DirectionToPlayer = Target->GetActorLocation() - DamageCauser->GetActorLocation();
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	params.Owner = DamageCauser;
	FVector Location = DamageCauser->GetActorLocation() + DamageCauser->GetActorForwardVector() * 10.f;
	ADebuffActor* DebuffActor = nullptr;
	if (ActorToSpawn) {
		DebuffActor = World->SpawnActor<ADebuffActor>(ActorToSpawn, Location, DirectionToPlayer.Rotation(), params);
	}
	if (DebuffActor && NiagaraSystem) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, NiagaraSystem, DebuffActor->GetActorLocation());
	}
	World->GetTimerManager().SetTimer(FDebuffTimerHandle, [this, DebuffActor]()
		{
			if (DebuffActor)
			{
				DebuffActor->Destroy();
			}
		}, DebuffTime, false);
}
