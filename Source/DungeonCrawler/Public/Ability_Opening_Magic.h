// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Opening_Magic.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Opening_Magic : public UAbility
{
	GENERATED_BODY()

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	FDelegateHandle FOpeningMagic;
	
};
