// Fill out your copyright notice in the Description page of Project Settings.


#include "EscortController.h"

#include "Navigation/PathFollowingComponent.h"

void AEscortController::StartEscort(FVector Location)
{
	MoveToLocation(Location,5.f);
}

void AEscortController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEscortController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	ReachedLocation = true;
}

