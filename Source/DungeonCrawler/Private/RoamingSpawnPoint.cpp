// Fill out your copyright notice in the Description page of Project Settings.


#include "RoamingSpawnPoint.h"

#include "Enemy.h"
#include "Components/SplineComponent.h"

ARoamingSpawnPoint::ARoamingSpawnPoint()
{
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("RoamingSpline"));
	Spline->SetupAttachment(RootComponent);
}

AEnemy* ARoamingSpawnPoint::SpawnEnemy()
{
	if(AEnemy*Enemy = Super::SpawnEnemy())
	{
		Enemy->EnemyState = EEnemyStates::roaming;
		Enemy->Owner = this;
		return Enemy;
	}

	return nullptr;

}
