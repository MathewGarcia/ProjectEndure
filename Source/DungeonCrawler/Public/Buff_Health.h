// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff.h"
#include "Buff_Health.generated.h"

/**
 * 
 */


enum class EPlayerStatType : uint8;

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UBuff_Health : public UBuff
{
	GENERATED_BODY()
public:
	UPROPERTY()
	EPlayerStatType StatType;

	virtual void Execute() override;

};
