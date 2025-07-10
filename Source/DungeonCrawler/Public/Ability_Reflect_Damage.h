// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Reflect_Damage.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Reflect_Damage : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Percent")
	float Percent = 0.1;

	float DamageTaken;

	UPROPERTY()
	AEnemy* DamageCauser;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

};
