// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "TreasureChest.generated.h"

class UTimelineComponent;
class AItem;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ATreasureChest : public AInteractableActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,Category = "Item To Spawn")
	TMap<TSubclassOf<AItem>,float> ItemToSpawn;

	UPROPERTY(EditAnywhere,Category = "Open Animation")
	UAnimSequence* OpenAnimation;

	UPROPERTY(EditAnywhere, Category = "Arc")
	float ArcHeight = 100.f;



	virtual void Interact_Implementation() override;

	TSubclassOf<AItem> RollForItemToSpawn();

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY()
	AItem* SpawnedItem = nullptr;

	FVector EndLocation;

	FVector StartLocation;

	bool bIsMoving = false;

	float currentAlpha = 0.f;

};
