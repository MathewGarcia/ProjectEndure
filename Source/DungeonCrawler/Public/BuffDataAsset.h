// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuffDataAsset.generated.h"

class UBuff;
class APlayerCharacter;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class DUNGEONCRAWLER_API UBuffDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere,Category = "BuffInfo")
	FText BuffName;

	UPROPERTY(EditAnywhere, Category = "BuffInfo")
	FText BuffDescription;

	UPROPERTY(EditAnywhere, Category = "BuffInfo")
	UTexture2D* ImageToShow;

	UPROPERTY(EditAnywhere,Category = "Buff Info")
	int BuffIndex;

	UPROPERTY(EditAnywhere, Category = "Buff Info")
	TSubclassOf<UBuff> BuffObjectToSpawn;

};
