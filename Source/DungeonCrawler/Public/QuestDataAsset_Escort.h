// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestDataAsset.h"
#include "QuestDataAsset_Escort.generated.h"

class ANPC;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuestDataAsset_Escort : public UQuestDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Escort NPC")
	TSubclassOf<ANPC> EscortNPC;

	UPROPERTY(EditAnywhere, Category = "Escort NPC Goal")
	TSubclassOf<AActor> GoalActor;
};
