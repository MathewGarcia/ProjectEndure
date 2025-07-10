// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearItemInstance.h"
#include "ItemDataObject.h"
#include "GearItemInstanceSaveData.generated.h"

/**
 * 
 */
USTRUCT()
struct DUNGEONCRAWLER_API FGearItemInstanceSaveData
{
	GENERATED_BODY()


	UPROPERTY()
	FString GearID;
	UPROPERTY()
	int Strength = 0;
	UPROPERTY()
	int Intellect = 0;
	UPROPERTY()
	int GearLevel = 0;
	UPROPERTY()
	int Stamina = 0;
	UPROPERTY()
	int Health = 0;
	UPROPERTY()
	float PhysicalProtection = 0.f;
	UPROPERTY()
	float PoiseProtection = 0.f;

	UPROPERTY()
	int Quantity;

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

	UPROPERTY()
	int IronCoreResourceAmt = 0;

	UPROPERTY()
	int IronFragmentResourceAmt = 0;

	UPROPERTY()
	int IronStoneResourceAmt = 0;

	UPROPERTY()
	int ElementalCoreResourceAmt = 0;

	UPROPERTY()
	int EXPNeeded = 0;


	void InitGear(UGearItemInstance*GearItemInstance)
	{
		if (!GearItemInstance) return;


		GearItemInstance->Strength = Strength;
		GearItemInstance->Intellect = Intellect;
		GearItemInstance->GearLevel = GearLevel;
		GearItemInstance->Stamina = Stamina;
		GearItemInstance->Health = Health;

		GearItemInstance->PhysicalProtection = PhysicalProtection;
		GearItemInstance->PoiseProtection = PoiseProtection;

		GearItemInstance->WaterResistance = WaterResistance;
		GearItemInstance->FireResistance = FireResistance;
		GearItemInstance->LighteningResistance = LighteningResistance;
		GearItemInstance->ShadowResistance = ShadowResistance;
		GearItemInstance->BleedResistance = BleedResistance;
		GearItemInstance->PoisonResistance = PoisonResistance;

		GearItemInstance->IronCoreResourceAmt = IronCoreResourceAmt;
		GearItemInstance->IronFragmentResourceAmt = IronFragmentResourceAmt;
		GearItemInstance->IronStoneResourceAmt = IronStoneResourceAmt;
		GearItemInstance->ElementalCoreResourceAmt = ElementalCoreResourceAmt;

		GearItemInstance->EXPNeeded = EXPNeeded;
		GearItemInstance->SetQuantity(Quantity);
	}
};
