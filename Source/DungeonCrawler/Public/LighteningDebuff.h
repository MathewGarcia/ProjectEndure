// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "LighteningDebuff.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API ULighteningDebuff : public UElementDebuff
{
	GENERATED_BODY()

public:
	virtual void ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser) override;
};
