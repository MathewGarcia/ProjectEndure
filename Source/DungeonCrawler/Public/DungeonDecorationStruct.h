// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonPieceActor.h"
#include "DungeonDecorationStruct.generated.h"

/**
 * 
 */

UENUM()
enum class EDecorationType : uint8
{
	Center UMETA(DisplayName = "Center"),
	Wallside UMETA(DisplayName = "Wallside")
};

USTRUCT()
struct DUNGEONCRAWLER_API FDungeonDecoRunTime
{
	GENERATED_BODY()

	float CalculatedBudget;

	TArray<FVector> LastPlaced;

};

class AEnvironmentalItem;
USTRUCT(Blueprintable)
struct DUNGEONCRAWLER_API FDungeonDecorationStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Map Decorations")
	EPieceType PieceType;

	UPROPERTY(EditAnywhere,Category = "Map Decorations")
	TSubclassOf<AEnvironmentalItem> DecorationPiece;

	UPROPERTY(EditAnywhere, Category = "Map Decorations")
	EDecorationType Type;

	UPROPERTY(EditAnywhere,Category = "Map Decorations")
	float Density = 0.03;

	UPROPERTY(EditAnywhere, Category = "Map Decorations")
	int32 MinRooms = 1;

};
