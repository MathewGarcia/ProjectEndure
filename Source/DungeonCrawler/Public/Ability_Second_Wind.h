// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Second_Wind.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Second_Wind : public UAbility
{
	GENERATED_BODY()

public:
	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;
	
};
