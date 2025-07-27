// Fill out your copyright notice in the Description page of Project Settings.


#include "GearItemInstance.h"

#include "GearItemData.h"
#include "GearItemInstanceSaveData.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"


void UGearItemInstance::InitializeStats()
{
	if (UGearItemData* GearItemData = Cast<UGearItemData>(ItemData)) {

		Strength = GearItemData->Strength;
		Intellect = GearItemData->Intellect;
		Stamina = GearItemData->Stamina;
		Quantity = ItemData->Quantity;
		Health = GearItemData->Health;
		PoiseProtection = GearItemData->PoiseProtection;
		WaterResistance = GearItemData->WaterResistance;
		FireResistance = GearItemData->FireResistance;
		LighteningResistance = GearItemData->LighteningResistance;
		ShadowResistance = GearItemData->ShadowResistance;
		BleedResistance = GearItemData->BleedResistance;
		PoisonResistance = GearItemData->PoisonResistance;
		IronCoreResourceAmt = GearItemData->IronCoreResourceAmt;
		IronFragmentResourceAmt = GearItemData->IronFragmentResourceAmt;
		IronStoneResourceAmt = GearItemData->IronStoneResourceAmt;
		ElementalCoreResourceAmt = GearItemData->ElementalCoreResourceAmt;
		EXPNeeded = GearItemData->EXPNeeded;
	}
}

void UGearItemInstance::UpgradeGear()
{
	if (UGearItemData* GearItemData = Cast<UGearItemData>(ItemData)) {



		if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOuter()))
		{
			for (const auto& itemToRemove : ItemsToRemove)
			{
				pcs->RemoveItemFromInventory(itemToRemove.Key, itemToRemove.Value);
			}

			pcs->playerStats.UpdateEXP(-EXPNeeded);

			if (APlayerCharacter* player = Cast<APlayerCharacter>(pcs->GetPawn())) {
				player->UpdateEXPWidget();
			}

		}

		++GearLevel;
		Strength = GearItemData->UpgradeStat(Strength, GearLevel, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Intellect = GearItemData->UpgradeStat(Intellect, GearLevel, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Stamina = GearItemData->UpgradeStat(Stamina, GearLevel, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Health = GearItemData->UpgradeStat(Strength, GearLevel, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		PoiseProtection = GearItemData->UpgradeResistance(GearItemData->MaxPoisonResistance, GearItemData->GearResistanceStep, GearLevel, PoiseProtection);
		WaterResistance = GearItemData->UpgradeResistance(GearItemData->MaxWaterResistance, GearItemData->GearResistanceStep, GearLevel, WaterResistance);
		FireResistance = GearItemData->UpgradeResistance(GearItemData->MaxFireResistance, GearItemData->GearResistanceStep, GearLevel, FireResistance);
		LighteningResistance = GearItemData->UpgradeResistance(GearItemData->MaxLighteningResistance, GearItemData->GearResistanceStep, GearLevel, LighteningResistance);
		ShadowResistance = GearItemData->UpgradeResistance(GearItemData->MaxShadowResistance, GearItemData->GearResistanceStep, GearLevel, ShadowResistance);
		BleedResistance = GearItemData->UpgradeResistance(GearItemData->MaxBleedResistance, GearItemData->GearResistanceStep, GearLevel, BleedResistance);
		PoisonResistance = GearItemData->UpgradeResistance(GearItemData->MaxPoisonResistance, GearItemData->GearResistanceStep, GearLevel, PoisonResistance);
		IronCoreResourceAmt = GearItemData->IncreaseResourceAmt(10, GearItemData->IronCoreResourceAmt, GearLevel, 0);
		EXPNeeded = GearItemData->IncreaseResourceAmt(80000, GearItemData->EXPNeeded, GearLevel, 0);
		IronFragmentResourceAmt = GearItemData->IncreaseResourceAmt(8, GearItemData->IronFragmentResourceAmt, GearLevel, 5);
		IronStoneResourceAmt = GearItemData->IncreaseResourceAmt(5, GearItemData->IronStoneResourceAmt, GearLevel, 10);
	}


}

bool UGearItemInstance::bCanUpgrade()
{
	if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOuter()))
	{
		for (UItemDataObject* item : pcs->GetInventory())
		{
			if (!item || !item->ItemData) continue;
			if (item->ItemData->ItemName.ToString().Equals("Iron Core"))
			{
				if (item->Quantity < IronCoreResourceAmt)
				{
					ItemsToRemove.Empty();
					return false;
				}
				ItemsToRemove.Add(item, IronCoreResourceAmt);
			}
			else if (item->ItemData->ItemName.ToString().Equals("Iron Fragment"))
			{
				if (item->Quantity < IronFragmentResourceAmt)
				{
					ItemsToRemove.Empty();
					return false;
				}
				ItemsToRemove.Add(item, IronFragmentResourceAmt);
			}
			else if (item->ItemData->ItemName.ToString().Equals("Iron Stone"))
			{
				if (item->Quantity < IronStoneResourceAmt)
				{
					ItemsToRemove.Empty();
					return false;
				}
				ItemsToRemove.Add(item, IronStoneResourceAmt);

			}
			else if (item->ItemData->ItemName.ToString().Equals("Elemental Core"))
			{
				if (item->Quantity < ElementalCoreResourceAmt)
				{
					ItemsToRemove.Empty();
					return false;
				}
				ItemsToRemove.Add(item, ElementalCoreResourceAmt);
			}
		}
		if (pcs->playerStats.totalEXPGained < EXPNeeded)
		{
			ItemsToRemove.Empty();
			return false;
		}

		return true;
	}
	ItemsToRemove.Empty();
	return false;
}

void UGearItemInstance::SeralizeGearItem(FGearItemInstanceSaveData& Output)
{

	if (UGearItemData* GearItemData = Cast<UGearItemData>(ItemData)) {


		Output.GearID = GearItemData->ItemName.ToString();
		Output.GearLevel = GearLevel;
		Output.Strength = Strength;
		Output.Intellect = Intellect;
		Output.Stamina = Stamina;
		Output.Health = Health;
		Output.PhysicalProtection = PhysicalProtection;
		Output.PoiseProtection = PoiseProtection;
		Output.WaterResistance = WaterResistance;
		Output.FireResistance = FireResistance;
		Output.LighteningResistance = LighteningResistance;
		Output.ShadowResistance = ShadowResistance;
		Output.BleedResistance = BleedResistance;
		Output.PoisonResistance = PoisonResistance;
		Output.IronCoreResourceAmt = IronCoreResourceAmt;
		Output.IronFragmentResourceAmt = IronFragmentResourceAmt;
		Output.IronStoneResourceAmt = IronStoneResourceAmt;
		Output.ElementalCoreResourceAmt = ElementalCoreResourceAmt;
		Output.EXPNeeded = EXPNeeded;
		Output.Quantity = ItemData->Quantity;
	}
	
}

void UGearItemInstance::InitStatsAccordingToLevel(int Level)
{
	if (Level == 0)
	{
		InitializeStats();
		return;
	}

	if (UGearItemData* GearItemData = Cast<UGearItemData>(ItemData)) {


		GearLevel = Level;
		Strength = GearItemData->UpgradeStat(GearItemData->Strength, Level, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Intellect = GearItemData->UpgradeStat(GearItemData->Intellect, Level, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Stamina = GearItemData->UpgradeStat(GearItemData->Stamina, Level, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		Health = GearItemData->UpgradeStat(GearItemData->Strength, Level, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
		PoiseProtection = GearItemData->UpgradeResistance(GearItemData->MaxPoisonResistance, GearItemData->GearResistanceStep, Level, GearItemData->PoiseProtection);
		WaterResistance = GearItemData->UpgradeResistance(GearItemData->MaxWaterResistance, GearItemData->GearResistanceStep, Level, GearItemData->WaterResistance);
		FireResistance = GearItemData->UpgradeResistance(GearItemData->MaxFireResistance, GearItemData->GearResistanceStep, Level, GearItemData->FireResistance);
		LighteningResistance = GearItemData->UpgradeResistance(GearItemData->MaxLighteningResistance, GearItemData->GearResistanceStep, Level, GearItemData->LighteningResistance);
		ShadowResistance = GearItemData->UpgradeResistance(GearItemData->MaxShadowResistance, GearItemData->GearResistanceStep, Level, GearItemData->ShadowResistance);
		BleedResistance = GearItemData->UpgradeResistance(GearItemData->MaxBleedResistance, GearItemData->GearResistanceStep, Level, GearItemData->BleedResistance);
		PoisonResistance = GearItemData->UpgradeResistance(GearItemData->MaxPoisonResistance, GearItemData->GearResistanceStep, Level, GearItemData->PoisonResistance);
		IronCoreResourceAmt = GearItemData->IncreaseResourceAmt(10, GearItemData->IronCoreResourceAmt, Level, 0);
		EXPNeeded = GearItemData->IncreaseResourceAmt(80000, GearItemData->EXPNeeded, Level, 0);
		IronFragmentResourceAmt = GearItemData->IncreaseResourceAmt(8, GearItemData->IronFragmentResourceAmt, Level, 5);
		IronStoneResourceAmt = GearItemData->IncreaseResourceAmt(5, GearItemData->IronStoneResourceAmt, Level, 10);
	}

}
