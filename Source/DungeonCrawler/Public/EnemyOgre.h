// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "EnemyOgre.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API AEnemyOgre : public AEnemy
{
	GENERATED_BODY()

public:

	AEnemyOgre();


protected:

	void BeginPlay() override;
};
