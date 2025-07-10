// Fill out your copyright notice in the Description page of Project Settings.


#include "BlacksmithItemWidget.h"

#include "BlacksmithUpgradeUI.h"
#include "GearItemData.h"
#include "GearItemInstance.h"
#include "ItemDataObject.h"
#include "WeaponInstance.h"
#include "WeaponItemData.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBlacksmithItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	if (UItemDataObject* ItemDataObject = Cast<UItemDataObject>(ListItemObject))
	{
		if (!ItemDataObject || !ItemDataObject->ItemData) return;

		IDO = ItemDataObject;
		ItemData = ItemDataObject->ItemData;

		if (ItemData)
		{
			if (UGearItemInstance* GearItem = Cast<UGearItemInstance>(ItemDataObject))
			{
				SetGearInformation(GearItem);
			}
			else if (UWeaponInstance* WeaponInstance= Cast<UWeaponInstance>(ItemDataObject))
			{
				if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(ItemData)) {
					if (WeaponHB && GearHB)
					{
						WeaponHB->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
						GearHB->SetVisibility(ESlateVisibility::Hidden);
					}
					if (ItemName)
					{
						ItemName->SetText(ItemData->ItemName);
					}

					if (ItemImage) {
						ItemImage->SetBrushFromTexture(ItemData->Image);
					}

					if (CurrentWeaponLevel)
					{
						CurrentWeaponLevel->SetText(FText::FromString(FString::FromInt(WeaponInstance->CurrentWeaponLevel)));
					}
					if (NewWeaponLevel)
					{
						int newGearLevel = WeaponInstance->CurrentWeaponLevel + 1;
						NewWeaponLevel->SetText(FText::FromString(FString::FromInt(newGearLevel)));
					}
					if (CurrentBaseDamage)
					{
						CurrentBaseDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->BaseDamage)));
					}
					if (NewBaseDamage)
					{
						float NewBaseDamageVal = WeaponItemData->UpgradeStat(WeaponInstance->BaseDamage, WeaponInstance->CurrentWeaponLevel + 1, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
						NewBaseDamage->SetText(FText::FromString(FString::FromInt(NewBaseDamageVal)));
					}
					if (CurrentStrength)
					{
						CurrentStrength->SetText(FText::FromString(FString::FromInt(WeaponInstance->Strength)));
					}
					if (NewStrength)
					{
						float NewStrengthVal = WeaponItemData->UpgradeStat(WeaponInstance->Strength, WeaponInstance->CurrentWeaponLevel + 1, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
						NewStrength->SetText(FText::FromString(FString::FromInt(NewStrengthVal)));
					}
					if (CurrentIntellect)
					{
						CurrentIntellect->SetText(FText::FromString(FString::FromInt(WeaponInstance->Intellect)));
					}
					if (NewIntellect)
					{
						float NewIntellectVal = WeaponItemData->UpgradeStat(WeaponInstance->Intellect, WeaponInstance->CurrentWeaponLevel + 1, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
						NewIntellect->SetText(FText::FromString(FString::FromInt(NewIntellectVal)));
					}

					if (CurrentElement)
					{
						CurrentElement->SetText(FText::FromString(UEnum::GetValueAsString(WeaponInstance->WeaponElementType).RightChop(FString("EElementTypes::").Len())));
					}

					if (CurrentElementDamage)
					{
						CurrentElementDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->ElementTypeDamage)));
					}

					if (NewElementDamage)
					{
						float NewElementDamageVal = WeaponItemData->UpgradeStat(WeaponInstance->ElementTypeDamage, WeaponInstance->CurrentWeaponLevel + 1, WeaponItemData->WeaponUpgradeStep, WeaponItemData->WeaponUpgradeExponent);
						NewElementDamage->SetText(FText::FromString(FString::FromInt(NewElementDamageVal)));
					}

					if (IronCoreCost)
					{
						IronCoreCost->SetText(FText::FromString(FString::FromInt(WeaponInstance->IronCoreResourceAmt)));
					}
					if (IronFragmentCost)
					{
						IronFragmentCost->SetText(FText::FromString(FString::FromInt(WeaponInstance->IronFragmentResourceAmt)));
					}
					if (IronStoneCost)
					{
						IronStoneCost->SetText(FText::FromString(FString::FromInt(WeaponInstance->IronStoneResourceAmt)));
					}
					if (EXPCost)
					{
						EXPCost->SetText(FText::FromString(FString::FromInt(WeaponInstance->EXPNeeded)));
					}
					if (ElementCost)
					{
						ElementCost->SetText(FText::FromString(FString::FromInt(WeaponInstance->ElementalCoreResourceAmt)));
					}
				}
			}
		}
	}
}

void UBlacksmithItemWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UBlacksmithItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);


	if(BackgroundImage)
	{
		BackgroundImage->SetColorAndOpacity(FLinearColor::Yellow);
	}
}

void UBlacksmithItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (ParentWidget && ParentWidget->UpgradeItemData && ParentWidget->UpgradeItemData != IDO && BackgroundImage)
	{
		BackgroundImage->SetColorAndOpacity(FLinearColor::Black);
	}
	else if(ParentWidget && !ParentWidget->UpgradeItemData && BackgroundImage)
	{
		BackgroundImage->SetColorAndOpacity(FLinearColor::Black);

	}
}

void UBlacksmithItemWidget::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

	if (BackgroundImage) {
		if (bIsSelected)
		{
			BackgroundImage->SetColorAndOpacity(FLinearColor::Yellow);
		}
		else
		{
			BackgroundImage->SetColorAndOpacity(FLinearColor::Black);

		}
	}
}

void UBlacksmithItemWidget::SetParentWidget(UBlacksmithUpgradeUI* NewParentWidget)
{

	ParentWidget = NewParentWidget;
}

void UBlacksmithItemWidget::SetGearInformation(UGearItemInstance* GearItemInstance)
{

	if (UGearItemData* GearItemData = Cast<UGearItemData>(GearItemInstance->ItemData)) {

		if (WeaponHB && GearHB)
		{
			WeaponHB->SetVisibility(ESlateVisibility::Hidden);
			GearHB->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		if (ItemName)
		{
			ItemName->SetText(ItemData->ItemName);
		}

		if (ItemImage) {
			ItemImage->SetBrushFromTexture(ItemData->Image);
		}

		if (CurrentGearLevel)
		{
			CurrentGearLevel->SetText(FText::FromString(FString::FromInt(GearItemInstance->GearLevel)));
		}
		if (NewGearLevel)
		{
			int newGearLevel = GearItemInstance->GearLevel + 1;
			NewGearLevel->SetText(FText::FromString(FString::FromInt(newGearLevel)));
		}

		if (CurrentGearHealth)
		{
			CurrentGearHealth->SetText(FText::FromString(FString::FromInt(GearItemInstance->Health)));
		}

		if (NewGearHealth)
		{
			int NewHealthVal = GearItemData->UpgradeStat(GearItemInstance->Health, GearItemInstance->GearLevel + 1, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
			NewGearHealth->SetText(FText::FromString(FString::FromInt(NewHealthVal)));
		}


		if (CurrentGearStamina)
		{
			CurrentGearStamina->SetText(FText::FromString(FString::FromInt(GearItemInstance->Stamina)));

		}

		if (NewGearStamina)
		{
			int NewStaminaVal = GearItemData->UpgradeStat(GearItemInstance->Stamina, GearItemInstance->GearLevel + 1, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
			NewGearStamina->SetText(FText::FromString(FString::FromInt(NewStaminaVal)));
		}
		if (CurrentGearStrength)
		{
			CurrentGearStrength->SetText(FText::FromString(FString::FromInt(GearItemInstance->Strength)));
		}
		if (NewGearStrength)
		{
			float NewStrengthVal = GearItemData->UpgradeStat(GearItemInstance->Strength, GearItemInstance->GearLevel + 1, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
			NewGearStrength->SetText(FText::FromString(FString::FromInt(NewStrengthVal)));
		}
		if (CurrentGearIntellect)
		{
			CurrentGearIntellect->SetText(FText::FromString(FString::FromInt(GearItemInstance->Intellect)));
		}
		if (NewGearIntellect)
		{
			float NewIntellectVal = GearItemData->UpgradeStat(GearItemInstance->Intellect, GearItemInstance->GearLevel + 1, GearItemData->GearUpgradeStep, GearItemData->GearUpgradeExponent);
			NewGearIntellect->SetText(FText::FromString(FString::FromInt(NewIntellectVal)));
		}


		if (CurrentGearPhysicalProtection)
		{
			CurrentGearPhysicalProtection->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PhysicalProtection)));
		}

		if (NewGearPhysicalResistance)
		{
			float NewGearPhysicalResistanceVal = GearItemData->UpgradeResistance(0.9, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->PhysicalProtection);
			NewGearPhysicalResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearPhysicalResistanceVal)));
		}

		if (CurrentGearPoiseProtection)
		{
			CurrentGearPhysicalProtection->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PoiseProtection)));

		}

		if (NewGearPoiseResistance)
		{
			float NewGearPoiseResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxPoiseProtection, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->PoiseProtection);
			NewGearPoiseResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearPoiseResistanceVal)));
		}

		if (CurrentGearWaterResistance)
		{
			CurrentGearWaterResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->WaterResistance)));
		}

		if (NewGearWaterResistance)
		{
			float NewGearWaterResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxWaterResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->WaterResistance);
			NewGearWaterResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearWaterResistanceVal)));
		}

		if (CurrentGearFireResistance)
		{
			CurrentGearFireResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->FireResistance)));
		}

		if (NewGearFireResistance)
		{
			float NewGearFireResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxFireResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->FireResistance);
			NewGearFireResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearFireResistanceVal)));
		}

		if (CurrentGearLightningResistance)
		{
			CurrentGearLightningResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->LighteningResistance)));
		}

		if (NewGearLightningResistance)
		{
			float NewGearLightningResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxLighteningResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->LighteningResistance);
			NewGearLightningResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearLightningResistanceVal)));
		}

		if (CurrentGearShadowResistance)
		{
			CurrentGearShadowResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->ShadowResistance)));
		}

		if (NewGearShadowResistance)
		{
			float NewGearShadowResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxShadowResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->ShadowResistance);
			NewGearShadowResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearShadowResistanceVal)));
		}

		if (CurrentGearBleedResistance)
		{
			CurrentGearBleedResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->BleedResistance)));
		}

		if (NewGearBleedResistance)
		{
			float NewGearBleedResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxBleedResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->BleedResistance);
			NewGearBleedResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearBleedResistanceVal)));
		}


		if (CurrentGearPoisonResistance)
		{
			CurrentGearPoisonResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PoisonResistance)));
		}

		if (NewGearPoisonResistance)
		{
			float NewGearPoisonResistanceVal = GearItemData->UpgradeResistance(GearItemData->MaxPoisonResistance, GearItemData->GearResistanceStep, GearItemInstance->GearLevel + 1, GearItemInstance->PoisonResistance);
			NewGearPoisonResistance->SetText(FText::FromString(FString::SanitizeFloat(NewGearPoisonResistanceVal)));
		}

		if (IronCoreCost)
		{
			IronCoreCost->SetText(FText::FromString(FString::FromInt(GearItemInstance->IronCoreResourceAmt)));
		}
		if (IronFragmentCost)
		{
			IronFragmentCost->SetText(FText::FromString(FString::FromInt(GearItemInstance->IronFragmentResourceAmt)));
		}
		if (IronStoneCost)
		{
			IronStoneCost->SetText(FText::FromString(FString::FromInt(GearItemInstance->IronStoneResourceAmt)));
		}
		if (EXPCost)
		{
			EXPCost->SetText(FText::FromString(FString::FromInt(GearItemInstance->EXPNeeded)));
		}
		if (ElementCost)
		{
			ElementCost->SetText(FText::FromString(FString::FromInt(GearItemInstance->ElementalCoreResourceAmt)));
		}
	}
}
