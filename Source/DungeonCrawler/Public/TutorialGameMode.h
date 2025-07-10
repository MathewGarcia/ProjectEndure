// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;

	
};
