// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveStoneActor.h"

#include "MainGameInstance.h"
#include "NiagaraFunctionLibrary.h"

void ASaveStoneActor::Interact_Implementation()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		MGI->LoadType = ELoadType::SaveGame;
		MGI->SaveGameData();
	}

	if(NiagaraSystem)
	{
		FVector Location = GetActorLocation() + (GetActorUpVector() * 100.f);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraSystem,Location);
	}
}
