// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ElementTypes.h"
#include "Engine/DataAsset.h"
#include "LiquidDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class DUNGEONCRAWLER_API ULiquidDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "LiquidType")
	EElementTypes ElementType;

	UPROPERTY(EditAnywhere, Category = "LiquidType")
	float ElementResistanceDamageRate;

	UPROPERTY(EditAnywhere, Category = "LiquidType")
	float ElementRawDamage;
};
