#pragma once


#include "CoreMinimal.h"
#include "GearType.h"
#include "ItemData.h"
#include "GearItemData.generated.h"

class AItem;
class UImage;
class AWeapon;
class UTexture2D;
class UBaseItem;

UCLASS(Blueprintable)
class DUNGEONCRAWLER_API UGearItemData : public UItemDataAsset
{
	GENERATED_BODY()


public:

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int Health = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int Strength = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int Intellect = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int GearLevel = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int Stamina = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float PhysicalProtection = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gear Requirements")
	int ReqStrength = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Requirements")
	int ReqIntellect = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Poise")
	float PoiseProtection = 0.f;


	UPROPERTY(EditAnywhere, Category = "Gear Poise")
	float MaxPoiseProtection = 0.f;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxWaterResistance = 0.0; //slows enemy, if hit with fire the enemy will steam. Burning them causing a dot.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxFireResistance = 0.0; //Burn debuff is added causing the player to become weaker to physical attacks.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxLighteningResistance = 0.0; //cause a lightening strike to come down and stun the player.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxShadowResistance = 0.0; //Shadow hand appear attempting to strike the enemy.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxBleedResistance = 0.0; //Bleed causing 2x damage.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float MaxPoisonResistance = 0.0; //Damage over time is caused.



	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float WaterResistance = 0.0; //slows enemy, if hit with fire the enemy will steam. Burning them causing a dot.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float FireResistance = 0.0; //Burn debuff is added causing the player to become weaker to physical attacks.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float LighteningResistance = 0.0; //cause a lightening strike to come down and stun the player.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float ShadowResistance = 0.0; //Shadow hand appear attempting to strike the enemy.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float BleedResistance = 0.0; //Bleed causing 2x damage.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float PoisonResistance = 0.0; //Damage over time is caused.

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float GearUpgradeStep = 50.f;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float GearUpgradeExponent = 0.5;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	float GearResistanceStep = 0.1f;


	UPROPERTY(EditAnywhere, Category = "Gear Hand Type")
	EGearType GearType = EGearType::none;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int IronCoreResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int IronFragmentResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int IronStoneResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int ElementalCoreResourceAmt = 0;

	UPROPERTY(EditAnywhere, Category = "Gear Stats")
	int EXPNeeded = 0;


};
