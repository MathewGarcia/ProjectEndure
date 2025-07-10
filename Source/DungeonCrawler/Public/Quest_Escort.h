// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Quest_Escort.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuest_Escort : public UQuest
{
	GENERATED_BODY()

	virtual void StartQuest_Implementation() override;

	UFUNCTION()
	virtual void Execute() override;
};
