// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest.h"
#include "Quest_Slay_Enemies.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UQuest_Slay_Enemies : public UQuest
{
	GENERATED_BODY()

public:


	virtual void StartQuest_Implementation() override;

	UFUNCTION()
	void OnEnemyKilled(AEnemy* Enemy);
private:
	int Progress = 0;
};
