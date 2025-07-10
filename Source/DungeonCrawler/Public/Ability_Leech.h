// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Leech.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Leech : public UAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "Percent")
	float Percent = 0.1;

	float DamageDealt;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

};
