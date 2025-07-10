// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EscortController.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AEscortController : public AAIController
{
	GENERATED_BODY()

public:

	bool DidReachLocation() const
	{
		return ReachedLocation;
	}
	
	void StartEscort(FVector Location);


protected:
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:


	bool ReachedLocation = false;
};
