// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff.h"
#include "Buff_Cooldown_Reduction.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UBuff_Cooldown_Reduction : public UBuff
{
	GENERATED_BODY()

	virtual void Execute() override;
};
