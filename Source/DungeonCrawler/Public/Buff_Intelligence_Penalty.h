// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff.h"
#include "Buff_Intelligence_Penalty.generated.h"

class UAbility_Intelligence_Penalty;
/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBuff_Intelligence_Penalty : public UBuff
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere,Category = "Ability To Create")
	TSubclassOf<UAbility_Intelligence_Penalty> AbilityIntellClass;

	int Amount;
protected:
	virtual void Execute() override;
};
