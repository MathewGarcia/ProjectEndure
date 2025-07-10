// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestDataAsset.h"
#include "QuestDataAsset_Slay_Enemies.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuestDataAsset_Slay_Enemies : public UQuestDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Enemies to kill")
	int32 amountToKill;

	UPROPERTY(EditAnywhere,Category = "Enemy")
	TSubclassOf<AEnemy> EnemyClass;
};
