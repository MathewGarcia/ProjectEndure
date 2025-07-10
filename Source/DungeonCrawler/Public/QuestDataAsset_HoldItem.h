// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestDataAsset.h"
#include "QuestDataAsset_HoldItem.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuestDataAsset_HoldItem : public UQuestDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly,Category = "Max Rounds")
	int32 MaxRounds;

};
