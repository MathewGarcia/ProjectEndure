#pragma once


#include "CoreMinimal.h"
#include "ItemData.h"
#include "ItemType.h"

#include "ConsumeableItemData.generated.h"

class AItem;
class UImage;
class AWeapon;
class UTexture2D;
class UBaseItem;

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API  UConsumeableItemData : public UItemDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Consumeable Strength Increase")
	float Strength;

	UPROPERTY(EditAnywhere, Category = "Consumeable Intellect Increase")
	float Intellect;

	UPROPERTY(EditAnywhere,Category = "Consumeable Health Increase")
	float Health;



};
