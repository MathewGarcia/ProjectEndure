// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponInstance.h"
#include "InventoryWidget.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "WeaponInstanceSaveData.h"
#include "WeaponItemData.h"

void UWeaponInstance::UpgradeWeapon()
{
	if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
		if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOuter()))
		{
			for (const auto& itemToRemove : ItemsToRemove)
			{
				if (itemToRemove.Key) {
					pcs->RemoveItemFromInventory(itemToRemove.Key, itemToRemove.Value);
				}
			}
			pcs->playerStats.UpdateEXP(-EXPNeeded);
			if(APlayerCharacter*player = Cast<APlayerCharacter>(pcs->GetPawn()))
			{
				player->UpdateEXPWidget();
			}
		}
		++CurrentWeaponLevel;
		Strength = WeaponItemData->UpgradeStat(WeaponItemData->Strength, CurrentWeaponLevel, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		BaseDamage = WeaponItemData->UpgradeStat(WeaponItemData->BaseDamage, CurrentWeaponLevel, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		Intellect = WeaponItemData->UpgradeStat(WeaponItemData->Intellect, CurrentWeaponLevel, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		ElementTypeDamage = WeaponItemData->UpgradeStat(WeaponItemData->ElementTypeDamage, CurrentWeaponLevel, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		IronCoreResourceAmt = WeaponItemData->IncreaseResourceAmt(10, WeaponItemData->IronCoreResourceAmt, CurrentWeaponLevel, 0);
		EXPNeeded = WeaponItemData->IncreaseResourceAmt(80000, WeaponItemData->EXPNeeded, CurrentWeaponLevel, 0);
		IronFragmentResourceAmt = WeaponItemData->IncreaseResourceAmt(8, WeaponItemData->IronFragmentResourceAmt, CurrentWeaponLevel, 5);
		IronStoneResourceAmt = WeaponItemData->IncreaseResourceAmt(5, WeaponItemData->IronStoneResourceAmt, CurrentWeaponLevel, 10);
		if (WeaponElementType != EElementTypes::Physical && WeaponElementType != EElementTypes::NONE)
		{
			ElementalCoreResourceAmt = WeaponItemData->IncreaseResourceAmt(1, WeaponItemData->ElementalCoreResourceAmt, CurrentWeaponLevel, 0);
		}
	}
}

void UWeaponInstance::InitAccordingToLevel(int Level)
{
	//if (Level == 0) return;
	if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
		CurrentWeaponLevel = Level;
		Strength = WeaponItemData->UpgradeStat(WeaponItemData->Strength, Level, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		BaseDamage = WeaponItemData->UpgradeStat(WeaponItemData->BaseDamage, Level, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		Intellect = WeaponItemData->UpgradeStat(WeaponItemData->Intellect, Level, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		ElementTypeDamage = WeaponItemData->UpgradeStat(ElementTypeDamage, Level, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
		IronCoreResourceAmt = WeaponItemData->IncreaseResourceAmt(10, WeaponItemData->IronCoreResourceAmt, Level, 0);
		EXPNeeded = WeaponItemData->IncreaseResourceAmt(80000, WeaponItemData->EXPNeeded, Level, 0);
		IronFragmentResourceAmt = WeaponItemData->IncreaseResourceAmt(8, WeaponItemData->IronFragmentResourceAmt, Level, 5);
		IronStoneResourceAmt = WeaponItemData->IncreaseResourceAmt(5, WeaponItemData->IronStoneResourceAmt, Level, 10);
		if (WeaponElementType != EElementTypes::Physical && WeaponElementType != EElementTypes::NONE)
		{
			ElementalCoreResourceAmt = WeaponItemData->IncreaseResourceAmt(1, WeaponItemData->ElementalCoreResourceAmt, Level, 0);
		}
	}
}

bool UWeaponInstance::bCanUpgrade()
{
	if (APlayerCharacterState* pcs = Cast<APlayerCharacterState>(GetOuter()))
	{
		ItemsToRemove.Empty();
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

void UWeaponInstance::SerializeWeapon(FWeaponInstanceSaveData& Output)
{
	if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
		Output.WeaponID = WeaponItemData->ItemName.ToString();
		Output.CurrentWeaponLevel = CurrentWeaponLevel;
		Output.Strength = Strength;
		Output.Intellect = Intellect;
		Output.BaseDamage = BaseDamage;
		Output.WeaponElementType = WeaponElementType;
		Output.ElementTypeDamage = ElementTypeDamage;
		Output.IronCoreResourceAmt = IronCoreResourceAmt;
		Output.IronFragmentResourceAmt = IronFragmentResourceAmt;
		Output.IronStoneResourceAmt = IronStoneResourceAmt;
		Output.ElementalCoreResourceAmt = ElementalCoreResourceAmt;
		Output.EXPNeeded = EXPNeeded;
		Output.Quantity = ItemData->Quantity;
	}
}

void UWeaponInstance::RandomlySetElementType()
{
	int RandomNum = FMath::RandRange(0, 6);
		switch(RandomNum)
		{
		case 0:
			WeaponElementType = EElementTypes::Physical;
			break;
		case 1:
			WeaponElementType = EElementTypes::Fire;
			break;
		case 2:
			WeaponElementType = EElementTypes::Shadow;
			break;
		case 3:
			WeaponElementType = EElementTypes::Water;
			break;
		case 4:
			WeaponElementType = EElementTypes::Lightening;
			break;
		case 5:
			WeaponElementType = EElementTypes::Bleed;
			break;
		case 6:
			WeaponElementType = EElementTypes::Poison;
			break;
		default:
			WeaponElementType = EElementTypes::Physical;
		}
		ElementTypeDamage += 1.f;
}
