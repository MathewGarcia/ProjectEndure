// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStats.h"
#include "UObject/NoExportTypes.h"
#include "GameStatsObject.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UGameStatsObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FGameStats GameStats;

};
