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

    if (ItemImage && ItemDataObject->ItemData->Image) {
        ItemImage->SetBrushFromTexture(ItemDataObject->ItemData->Image);
    }
    

    if(UWeaponInstance*WeaponInstance = Cast<UWeaponInstance>(ItemDataObject))
    {

        if (GearHB) {
            GearHB->SetVisibility(ESlateVisibility::Collapsed);
        }

        if (WeaponHB && (WeaponHB->GetVisibility() == ESlateVisibility::Hidden || WeaponHB->GetVisibility() == ESlateVisibility::Collapsed))
        {
            WeaponHB->SetVisibility(ESlateVisibility::Visible);
        }

        if (CurrentWeaponLevel) {
            CurrentWeaponLevel->SetText(FText::FromString(FString::FromInt(WeaponInstance->CurrentWeaponLevel)));
        }
        if (CurrentBaseDamage) {
            CurrentBaseDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->BaseDamage)));
        }
        if (CurrentStrength) {
            CurrentStrength->SetText(FText::FromString(FString::FromInt(WeaponInstance->Strength)));
        }
        if (CurrentIntellect) {
            CurrentIntellect->SetText(FText::FromString(FString::FromInt(WeaponInstance->Intellect)));
        }
        if (CurrentElement) {
            CurrentElement->SetText(FText::FromString(UEnum::GetValueAsString(WeaponInstance->WeaponElementType).RightChop(FString("EElementTypes::").Len())));
        }
        if (CurrentElementDamage) {
            CurrentElementDamage->SetText(FText::FromString(FString::FromInt(WeaponInstance->ElementTypeDamage)));
        }
        if(UWeaponItemData*WeaponItemData = Cast<UWeaponItemData>(WeaponInstance->ItemData))
        {
            if (RequiredStrength) {
                RequiredStrength->SetText(FText::FromString(FString::Printf(TEXT("Required Strength: %d"), WeaponItemData->ReqStrength)));
            }
            if (RequiredIntellect) {
                RequiredIntellect->SetText(FText::FromString(FString::Printf(TEXT("Required Intellect: %d"), WeaponItemData->ReqIntellect)));
            }
        }

    }
    else if (UGearItemInstance*GearItemInstance = Cast<UGearItemInstance>(ItemDataObject))
    {

        if (WeaponHB) {
            WeaponHB->SetVisibility(ESlateVisibility::Collapsed);
        }


        if (GearHB && (GearHB->GetVisibility() == ESlateVisibility::Hidden || GearHB->GetVisibility() == ESlateVisibility::Collapsed))
        {
            GearHB->SetVisibility(ESlateVisibility::Visible);
        }

        if (UGearItemData* GearItemData = Cast<UGearItemData>(GearItemInstance->ItemData))
        {
            if (RequiredStrength) {
                RequiredStrength->SetText(FText::FromString(FString::Printf(TEXT("Required Strength: %d"), GearItemData->ReqStrength)));
            }
            if (RequiredIntellect) {
                RequiredIntellect->SetText(FText::FromString(FString::Printf(TEXT("Required Intellect: %d"), GearItemData->ReqIntellect)));
            }
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
            CurrentGearPoiseProtection->SetText(FText::FromString(FString::SanitizeFloat(GearItemInstance->PoiseProtection)));

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
