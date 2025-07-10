// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "PoisonDebuff.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UPoisonDebuff : public UElementDebuff
{
	GENERATED_BODY()

public:
	virtual void ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser) override;

	FTimerHandle FInnerTimerHandle;

	UPROPERTY(EditAnywhere,Category = "Debuff")
	float TickRate = 1.f;

	virtual  void EndBuff() override;
};
