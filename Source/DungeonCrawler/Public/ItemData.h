#pragma once


#include "CoreMinimal.h"
#include "ItemType.h"
#include "ItemData.generated.h"

class AItem;
class UImage;
class AWeapon;
class UTexture2D;
class UBaseItem;

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,Category = "Inventory")
	FText ItemName;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Quantity;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Inventory")
	UTexture2D* Image;

	float PoiseProtection = 0.f;

	UPROPERTY(EditAnywhere, Category = "Item Logic Class")
	TSubclassOf<UBaseItem> ItemLogicClass;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Item To Spawn")
	TSubclassOf<AItem> ItemToSpawn;


	static float UpgradeStat(float Stat, int32 Level, float step, float exp)
	{
		if (Level <= 0 || Stat <= 0)
		{
			return Stat;
		}

		return Stat + step * FMath::Pow(Level, exp);
	};

	static float UpgradeResistance(float cap,float step,const int& currentGearLevel,float& currentResistance)
	{
		if(currentResistance <= 0)
		{
			return currentResistance;
		}

		currentResistance = cap * (1 - FMath::Exp(-step* currentGearLevel));
		return currentResistance;
	}

	inline static int IncreaseResourceAmt(int cap, int resource,int itemLevel,int UnlockLevel)
	{
		if(itemLevel < UnlockLevel)
		{
			return 0;
		}

		if (itemLevel == UnlockLevel && resource <= 0)
		{
			return 1;
		}
		float step = 0.2;

		
		return resource + (cap * (1-FMath::Exp(-step*itemLevel)));

	}
};
