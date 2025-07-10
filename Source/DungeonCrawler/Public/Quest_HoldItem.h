// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Quest_HoldItem.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuest_HoldItem : public UQuest
{
	GENERATED_BODY()

public:
	UFUNCTION()
	virtual void Execute() override;

	UFUNCTION()
	virtual void OnDeath() override;

	virtual void StartQuest_Implementation() override;

	virtual void SerializeQuest(FQuest_SaveData& Out) override;

	virtual  void DeserializeQuest(FQuest_SaveData* In) override;

private:

	int32 Rounds;


};
