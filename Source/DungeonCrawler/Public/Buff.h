// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff.generated.h"

class AEnemy;
/**
 * 
 */
UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UBuff : public UObject
{
	GENERATED_BODY()

public:

	virtual void Execute();

};
