// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "EmptyAbility.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UEmptyAbility : public UAbility
{
	GENERATED_BODY()
public:
	virtual void execute_Implementation();
};
