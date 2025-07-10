// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DungeonCrawlerGM.generated.h"

class APlayerCharacter;
class ADungeonManager;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundEnd);

UCLASS()
class DUNGEONCRAWLER_API ADungeonCrawlerGM : public AGameModeBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,Category = "Get Round Count")
	int GetRoundCount()
	{
		return roundCount;
	}

	void IncrementRoundCount(int NewRoundCount);

	void SetRoundCount(int NewRoundCount);

	void InitializeRound();

	TSoftObjectPtr<ADungeonManager> DungeonManagerWeakPTR;

	FOnRoundEnd OnRoundEndDelegate;

	bool bIsLoading = true;

	UFUNCTION()
	void Loading();

	TWeakObjectPtr<APlayerCharacter> SafePlayer;

	UFUNCTION()
	void SetPlayer(APlayerCharacter*player);

	bool bDungeonReady = false;

	UFUNCTION()
	void SetDungeonReady();

	void CheckReady();

	void InitializeTimer();

protected:
	virtual void BeginPlay() override;

private:
	int roundCount = 0;

	FTimerHandle FRemoveFogUITimer;
	
};
