// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ability.h"
#include "Ability_Arcane_Conduction.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UAbility_Arcane_Conduction : public UAbility
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,Category = "Return Percent")
	float Percent = 0.25;

	virtual void execute_Implementation() override;

	virtual bool bShouldExecute_Implementation() override;

	FDelegateHandle FArcaneConduactionDelegateHandle;
};
