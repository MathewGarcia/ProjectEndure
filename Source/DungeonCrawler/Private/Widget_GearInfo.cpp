// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_GearInfo.h"

#include "GearItemData.h"
#include "GearItemInstance.h"
#include "ItemDataObject.h"
#include "WeaponInstance.h"
#include "WeaponItemData.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWidget_GearInfo::SetInfo(UItemDataObject* ItemDataObject)
{
	if (!IsValid(ItemDataObject) || !IsValid(ItemDataObject->ItemData)) return;

	ItemImage->SetBrushFromTexture(ItemDataObject->ItemData->Image);
	

	if(UWeaponInstance*WeaponInstance = Cast<UWeaponInstance>(ItemDataObject))
	{

		GearHB->SetVisibility(ESlateVisibility::Collapsed);

		if(WeaponHB->GetVisibility() == ESlateVisibility::Hidden || WeaponHB->GetVisibility() == ESlateVisibility::Collapsed)
		{
			WeaponHB->SetVisibility(ESlateVisibility::Visible);
		}

		CurrentWeaponLevel->SetText(FText::FromString(FString::FromInt(WeaponInstance->CurrentWeaponLevel)));
		CurrentBaseDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->BaseDamage)));
		CurrentStrength->SetText(FText::FromString(FString::FromInt(WeaponInstance->Strength)));
		CurrentIntellect->SetText(FText::FromString(FString::FromInt(WeaponInstance->Intellect)));
		CurrentElement->SetText(FText::FromString(UEnum::GetValueAsString(WeaponInstance->WeaponElementType).RightChop(FString("EElementTypes::").Len())));
		CurrentElementDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->ElementTypeDamage)));
		if(UWeaponItemData*WeaponItemData = Cast<UWeaponItemData>(WeaponInstance->ItemData))
		{
			RequiredStrength->SetText(FText::FromString(FString::Printf(TEXT("Required Strength: %d"), WeaponItemData->ReqStrength)));
			RequiredIntellect->SetText(FText::FromString(FString::Printf(TEXT("Required Intellect: %d"), WeaponItemData->ReqIntellect)));

		}

	}
	else if (UGearItemInstance*GearItemInstance = Cast<UGearItemInstance>(ItemDataObject))
	{

		WeaponHB->SetVisibility(ESlateVisibility::Collapsed);


		if (GearHB->GetVisibility() == ESlateVisibility::Hidden || GearHB->GetVisibility() == ESlateVisibility::Collapsed)
		{
			GearHB->SetVisibility(ESlateVisibility::Visible);
		}

		if (UGearItemData* GearItemData = Cast<UGearItemData>(GearItemInstance->ItemData))
		{
			RequiredStrength->SetText(FText::FromString(FString::Printf(TEXT("Required Strength: %d"), GearItemData->ReqStrength)));
			RequiredIntellect->SetText(FText::FromString(FString::Printf(TEXT("Required Intellect: %d"), GearItemData->ReqIntellect)));

		}
		if (CurrentGearLevel)
		{
			CurrentGearLevel->SetText(FText::FromString(FString::FromInt(GearItemInstance->GearLevel)));
		}


		if (CurrentGearHealth)
		{
			CurrentGearHealth->SetText(FText::FromString(FString::FromInt(GearItemInstance->Health)));
		}



		if (CurrentGearStamina)
		{
			CurrentGearStamina->SetText(FText::FromString(FString::FromInt(GearItemInstance->Stamina)));

		}

	
		if (CurrentGearStrength)
		{
			CurrentGearStrength->SetText(FText::FromString(FString::FromInt(GearItemInstance->Strength)));
		}

		if (CurrentGearIntellect)
		{
			CurrentGearIntellect->SetText(FText::FromString(FString::FromInt(GearItemInstance->Intellect)));
		}


		if (CurrentGearPhysicalProtection)
		{
			CurrentGearPhysicalProtection->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PhysicalProtection)));
		}

		if (CurrentGearPoiseProtection)
		{
			CurrentGearPhysicalProtection->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PoiseProtection)));

		}


		if (CurrentGearWaterResistance)
		{
			CurrentGearWaterResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->WaterResistance)));
		}

	

		if (CurrentGearFireResistance)
		{
			CurrentGearFireResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->FireResistance)));
		}


		if (CurrentGearLightningResistance)
		{
			CurrentGearLightningResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->LighteningResistance)));
		}

		if (CurrentGearShadowResistance)
		{
			CurrentGearShadowResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->ShadowResistance)));
		}


		if (CurrentGearBleedResistance)
		{
			CurrentGearBleedResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->BleedResistance)));
		}



		if (CurrentGearPoisonResistance)
		{
			CurrentGearPoisonResistance->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PoisonResistance)));
		}

	}

}
