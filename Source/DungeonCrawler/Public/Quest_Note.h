// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Quest_Note.generated.h"

class ANPC;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuest_Note : public UQuest
{
	GENERATED_BODY()
	
	UFUNCTION()
	virtual void Execute() override;

	virtual void StartQuest_Implementation();

	UPROPERTY(EditAnywhere,Category = "Actor to Spawn")
	TSubclassOf<ANPC> ActorToSpawn;

	UPROPERTY(EditAnywhere,Category = "Roll needed")
	float RollNeeded = 0.5f;
};
