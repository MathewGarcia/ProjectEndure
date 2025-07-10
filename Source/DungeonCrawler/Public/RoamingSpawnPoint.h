// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnPoint.h"
#include "RoamingSpawnPoint.generated.h"

/**
 * 
 */
class USplineComponent;
UCLASS()
class DUNGEONCRAWLER_API ARoamingSpawnPoint : public ASpawnPoint
{
	GENERATED_BODY()

public:

	ARoamingSpawnPoint();

	UPROPERTY(EditAnywhere,Category = "Spline")
	USplineComponent* Spline;
	
	virtual AEnemy* SpawnEnemy() override;

	bool IsProcedural = false;

};
