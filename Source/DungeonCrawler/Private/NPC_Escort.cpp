// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC_Escort.h"

#include "EscortController.h"
#include "Quest_Escort.h"

void ANPC_Escort::StartEscort(FVector Location)
{
	if(AEscortController*EscortController = Cast<AEscortController>(GetController()))
	{
		EscortController->StartEscort(Location);
		bInitialized = true;
	}
}

void ANPC_Escort::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
}

void ANPC_Escort::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AEscortController* EscortController = Cast<AEscortController>(GetController()))
	{
		if(bInitialized && EscortController->DidReachLocation())
		{
			if(ParentQuest && !bAlreadyAwarded)
			{
				ParentQuest->CompleteQuest();
				EscortController->UnPossess();
				Destroy();
				bAlreadyAwarded = true;
			}
		}
	}

}
