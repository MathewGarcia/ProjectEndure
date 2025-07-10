#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.generated.h"

class UItemDataAsset;

USTRUCT()
struct DUNGEONCRAWLER_API FItemInstance
{
	GENERATED_BODY()

	UPROPERTY()
	FString ItemDataID;

	UPROPERTY()
	int32 Quantity;

};

