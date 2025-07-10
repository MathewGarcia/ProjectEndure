// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DungeonCrawlerGS.generated.h"

/**
 * 
 */
class APlayerCharacter;
UCLASS()
class DUNGEONCRAWLER_API ADungeonCrawlerGS : public AGameStateBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Player")
	APlayerCharacter* player;

protected:

	virtual void BeginPlay() override;
};
