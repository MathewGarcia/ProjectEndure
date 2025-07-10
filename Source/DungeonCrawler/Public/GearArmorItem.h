#pragma once

#include "CoreMinimal.h"
#include "GearItemData.h"
#include "GameFramework/Actor.h"
#include "Item.h"
#include "GearArmorItem.generated.h"


class UGearItemInstance;

UCLASS()
class DUNGEONCRAWLER_API AGearArmorItem : public AItem
{
	GENERATED_BODY()
	
public:	

	UPROPERTY()
	UGearItemData* GearItemData;

	UPROPERTY()
	UGearItemInstance* GearItemInstance;

	void InitGearItemInstance(int Level);

};