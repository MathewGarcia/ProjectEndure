// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest_SaveData.h"
#include "Quest.generated.h"

class UItemDataAsset;
class ANPC;
class UQuestDataAsset;
/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class DUNGEONCRAWLER_API UQuest : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	 void StartQuest();

	 virtual void StartQuest_Implementation();

	virtual void CompleteQuest();

	virtual void EndQuest();


	virtual void Execute();

	virtual void OnDeath();

	virtual void SerializeQuest(FQuest_SaveData& Out);

	virtual void DeserializeQuest(FQuest_SaveData* In);

	UItemDataAsset* GetReward();

	UPROPERTY()
	UQuestDataAsset* QuestDataAsset;

	UPROPERTY()
	ANPC* OwningNPC;
};
