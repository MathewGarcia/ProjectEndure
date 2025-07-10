// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementDebuff.h"
#include "FireDebuff.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONCRAWLER_API UFireDebuff : public UElementDebuff
{
	GENERATED_BODY()

public:
	virtual void ExecuteDebuff_Implementation(AActor* Target, AActor* DamageCauser) override;

	FTimerHandle FInnerTimerHandle;

	UPROPERTY(EditAnywhere,Category = "Burn Material")
	UMaterialInstance* BurnMaterial;

	virtual void EndBuff() override;

};
