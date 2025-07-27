// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerWidget.h"

#include <Components/Image.h>

#include "Ability.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "Widget_GearInfo.h"
#include "Components/ExpandableArea.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TileView.h"

void UMainPlayerWidget::WeaponUpdated(AWeapon*Weapon)
{
    if(Weapon && CrosshairImage)
    {
        switch (Weapon->WeaponType)
        {
        case EWeaponType::Bow:
            if (!CrosshairImage->IsVisible()) {
                CrosshairImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            }
            if (crosshairs.IsValidIndex(0) && crosshairs[0]) {
                CrosshairImage->SetBrushFromTexture(crosshairs[0]);
            }
            break;
        case EWeaponType::Mage:
            if (!CrosshairImage->IsVisible()) {
                CrosshairImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
            }
            if (crosshairs.IsValidIndex(1) && crosshairs[1]) {
                CrosshairImage->SetBrushFromTexture(crosshairs[1]);
            }
            break;
        default:
            if (CrosshairImage->IsVisible()) {
                CrosshairImage->SetVisibility(ESlateVisibility::Collapsed);
            }
            break;
        }
    }
}

void UMainPlayerWidget::AbilitiesUpdated()
{
    if (AbilityList) {
        AbilityList->ClearListItems();
        if (PCS) {
            for (UAbility*Ability : PCS->EquippedAbilities)
            {
                if (Ability) {
                    AbilityList->AddItem(Ability);
                }
            }
        }
    }
}

void UMainPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PCS = Cast<APlayerCharacterState>(GetOwningPlayerState());
    if(!PCS)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player Character state has failed"));
    }
    ProgressBars.Add({ "Health",Health });
    ProgressBars.Add({ "Stamina",Stamina });
    ProgressBars.Add({"Mana",Mana});
    ProgressBars.Add("BossHealth", BossHealth);
    ProgressBars.Add("Fire", Fire);
    ProgressBars.Add("Lightning", Lightning);
    ProgressBars.Add("Water", Water);
    ProgressBars.Add("Shadow", Shadow);
    ProgressBars.Add("Bleed", Bleed);
    ProgressBars.Add("Poison", Poison);
    SizeBoxMap.Add("Health", HealthSizeBox);
    SizeBoxMap.Add("Stamina", StaminaSizeBox);
    if (PCS) {
        PCS->OnAbilitiesUpdated.AddUniqueDynamic(this, &UMainPlayerWidget::AbilitiesUpdated);
        PCS->OnWeaponUpdated.AddUniqueDynamic(this, &UMainPlayerWidget::WeaponUpdated);
        if(APlayerCharacter*player = Cast<APlayerCharacter>(PCS->GetPawn()))
        {
            player->OnPlayerReady.AddLambda([this]
            {
                if (PCS) {
                    UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
                    UpdateProgressBar("Stamina", PCS->playerStats.currentStamina / PCS->GetTotalStamina());
                    UpdateProgressBar("Mana", PCS->playerStats.currentMana / PCS->GetTotalMana());
                    UpdateHealthSizeBox();
                    UpdateStaminaSizeBox();
                }
            });
        }
        if (EmptySlotAbilityClass) {
            for (int i = 0; i < 5; ++i)
            {
                UAbility* EmptyAbility = NewObject<UAbility>(PCS, EmptySlotAbilityClass);
                PCS->EquippedAbilities.Add(EmptyAbility);
            }
        }
        AbilitiesUpdated();
    }
}

void UMainPlayerWidget::UpdateProgressBar(FName ProgressBarToFind, const float& value)
{
    if(UProgressBar**FoundBar = ProgressBars.Find(ProgressBarToFind))
    {
        if(UProgressBar*Bar = *FoundBar)
        {
            Bar->SetPercent(value);
        }
    }
}

void UMainPlayerWidget::SetHorizontalBoxVisibility(EElementTypes ElementType, ESlateVisibility NewVisiblity)
{
    UHorizontalBox* ElementHB = nullptr;
    switch (ElementType)
    {
    case EElementTypes::Fire:
        ElementHB = FireHB;
        break;
    case EElementTypes::Water:
        ElementHB = WaterHB;
        break;
    case EElementTypes::Shadow:
        ElementHB = ShadowHB;
        break;
    case EElementTypes::Lightening:
        ElementHB = LightningHB;
        break;
    case EElementTypes::Bleed:
        ElementHB = BleedHB;
        break;
    case EElementTypes::Poison:
        ElementHB = PoisonHB;
        break;
    case EElementTypes::NONE:
        break;
    default:
        break;
    }
    if (ElementHB) {
        ElementHB->SetVisibility(NewVisiblity);
    }
}

void UMainPlayerWidget::RebindDelegates()
{
    if (PCS) {
        PCS->OnAbilitiesUpdated.AddUniqueDynamic(this, &UMainPlayerWidget::AbilitiesUpdated);
        PCS->OnWeaponUpdated.AddUniqueDynamic(this, &UMainPlayerWidget::WeaponUpdated);
    }
}

void UMainPlayerWidget::ExpandEquipment(bool NewVal)
{
    if(ExpandableEquipment)
    {
        ExpandableEquipment->SetIsExpanded(NewVal);
    }
}

void UMainPlayerWidget::ExpandStats(bool NewVal)
{
    if (ExpandableStats)
    {
        ExpandableStats->SetIsExpanded(NewVal);
    }
}

void UMainPlayerWidget::ExpandTalents(bool NewVal)
{
    if (ExpandableTalents)
    {
        ExpandableTalents->SetIsExpanded(NewVal);
    }
}

void UMainPlayerWidget::ExpandBag(bool NewVal)
{
    if (ExpandableBag)
    {
        if (!NewVal && GearInfo)
        {
            GearInfo->SetVisibility(ESlateVisibility::Collapsed);
        }
        ExpandableBag->SetIsExpanded(NewVal);
    }
}

void UMainPlayerWidget::SetLoadVisibility(ESlateVisibility NewVisibility)
{
    if(LoadScreenOverlay)
    {
        LoadScreenOverlay->SetVisibility(NewVisibility);
    }
}

void UMainPlayerWidget::UpdateHealthSizeBox()
{
    if(HealthSizeBox && PCS)
    {
        float WidthX = BaseWidth + (PCS->GetTotalHealth() - 333.33f);
        HealthSizeBox->SetWidthOverride(WidthX);
    }
}

void UMainPlayerWidget::UpdateStaminaSizeBox()
{
    if(StaminaSizeBox && PCS)
    {
        float WidthX = BaseWidth + (PCS->GetTotalStamina() - 250.f);
        StaminaSizeBox->SetWidthOverride(WidthX);
    }
}



