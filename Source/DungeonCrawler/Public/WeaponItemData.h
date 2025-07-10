#pragma once


#include "CoreMinimal.h"
#include "ItemData.h"
#include "ElementTypes.h"
#include "GearType.h"
#include "WeaponItemData.generated.h"

class AItem;
class UImage;
class AWeapon;
class UTexture2D;
class UBaseItem;

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UWeaponItemData : public UItemDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int Strength = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int Intellect = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int BaseDamage = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float StrengthScale = 0.0f;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float IntellectScale = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int GearLevel = 0;

	UPROPERTY(EditAnywhere, Category = "Weapon Requirements")
	int ReqStrength = 0;

	UPROPERTY(EditAnywhere, Category = "Weapon Requirements")
	int ReqIntellect = 0;

	UPROPERTY(EditAnywhere,Category = "Weapon Hand Type")
	EGearType GearType = EGearType::RHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Element Type")
	EElementTypes WeaponElementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Element Type")
	float ElementTypeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Element Type")
	float ElementTypeProgressionDamage = 0.f;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float WeaponUpgradeStep = 50.f;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	float WeaponUpgradeExponent = 0.5;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int IronCoreResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int IronFragmentResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int IronStoneResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int ElementalCoreResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "WeaponStats")
	int EXPNeeded = 0;

};
