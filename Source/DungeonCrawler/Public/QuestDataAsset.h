// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "QuestDataAsset.generated.h"

class UQuest;
class UItemDataAsset;
/**
 * 
 */
UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Quest Info")
	FText QuestName;


	UPROPERTY(EditAnywhere, Category = "Quest Info")
	FText QuestDescription;


	UPROPERTY(EditAnywhere, Category = "Quest Info")
	TMap<UItemDataAsset*,float>RewardsMap;


	UPROPERTY(EditAnywhere, Category = "Quest Info")
	float RewardEXP;

	UPROPERTY(EditAnywhere, Category = "Quest Info")
	TSubclassOf<UQuest> QuestToSpawn;

};
