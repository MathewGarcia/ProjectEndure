// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BoxTrigger.h"
#include "BoxTrigger_Reset.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ABoxTrigger_Reset : public ABoxTrigger
{
	GENERATED_BODY()


	virtual void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
