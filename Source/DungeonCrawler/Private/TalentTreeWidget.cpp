// Fill out your copyright notice in the Description page of Project Settings.

#include "TalentTreeWidget.h"

#include "InGamePlayerHUD.h"
#include "PlayerCharacterState.h"
#include "Components/HorizontalBox.h"
#include "PlayerCharacter.h"
#include "Components/VerticalBox.h"

void UTalentTreeWidget::fillRows(TArray<UHorizontalBox*>& rowToFill, UVerticalBox* rowToSearch)
{
    if (!rowToSearch) return;
    for (auto* child : rowToSearch->GetAllChildren())
    {
        if (UHorizontalBox* newRow = Cast<UHorizontalBox>(child)) {
            rowToFill.Add(newRow);
        }
    }
}

void UTalentTreeWidget::EnableRow(TArray<UHorizontalBox*> rowToCheck,const int& playerStat)
{
    for(int i = 0; i < rowToCheck.Num(); ++i)
    {
        if(PCS && rowToCheck[i])
        {
            switch (i)
            {
            case 0:
                if (playerStat >= 10) {
                    if (!rowToCheck[i]->GetIsEnabled()) {
                        rowToCheck[i]->SetIsEnabled(true);
                        rowToCheck[i]->SetRenderOpacity(1.0f);
                    }
                }
                break;
            case 1:
                if (playerStat >= 20) {
                    if (!rowToCheck[i]->GetIsEnabled()) {
                        rowToCheck[i]->SetIsEnabled(true);
                        rowToCheck[i]->SetRenderOpacity(1.0f);
                    }
                }
                break;
            case 2:
                if (playerStat >= 30) {
                    if (!rowToCheck[i]->GetIsEnabled()) {
                        rowToCheck[i]->SetIsEnabled(true);
                        rowToCheck[i]->SetRenderOpacity(1.0f);
                    }
                }
                break;
            case 3:
                if (playerStat >= 40) {
                    if (!rowToCheck[i]->GetIsEnabled()) {
                        rowToCheck[i]->SetIsEnabled(true);
                        rowToCheck[i]->SetRenderOpacity(1.0f);
                    }
                }
                break;
            default: break;
            }
        }
    }
}

void UTalentTreeWidget::setAllRowsDisabled(TArray<UHorizontalBox*> rowToHide)
{
    for(UHorizontalBox*row : rowToHide)
    {
        if (row) {
            row->SetRenderOpacity(0.5);
            row->SetIsEnabled(false);
        }
    }
}

void UTalentTreeWidget::CheckAllRows()
{
    if (PCS) {
        EnableRow(StrengthRows, PCS->playerStats.Strength);
        EnableRow(IntellectRows, PCS->playerStats.Intellect);
    }
}

void UTalentTreeWidget::NativeConstruct()
{
    Super::NativeConstruct();
    PCS = Cast<APlayerCharacterState>(GetOwningPlayerState());
    Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
    if (Player)
    {
        if (APlayerController* PC = Cast<APlayerController>(Player->Controller))
        {
            PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD());
            if (PlayerHUD)
            {
                PlayerHUD->TalentTreeWidget = this;
            }
        }
    }
    if(!PCS)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player State failed"));
    }
    if (!StrengthVB || !IntellectVB) return;
    fillRows(StrengthRows,StrengthVB);
    fillRows(IntellectRows, IntellectVB);
    if(StrengthRows.IsEmpty() || IntellectRows.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Talent Tree Rows are empty"));
        return;
    }
    setAllRowsDisabled(StrengthRows);
    setAllRowsDisabled(IntellectRows);
    if (PCS) {
        EnableRow(StrengthRows, PCS->playerStats.Strength);
        EnableRow(IntellectRows, PCS->playerStats.Intellect);
    }
    SetTalentWidgetOwner();
}

void UTalentTreeWidget::SetTalentWidgetOwner()
{
    for(auto*Widget : StrengthRows)
    {
        if (!Widget) continue;
        for(auto*child : Widget->GetAllChildren())
        {
            if (UTalentWidget* TalentWidget = Cast<UTalentWidget>(child))
            {
                TalentWidget->OwningWidget = this;
            }
        }
    }
    for (auto* Widget : IntellectRows)
    {
        if (!Widget) continue;
        for (auto* child : Widget->GetAllChildren())
        {
            if (UTalentWidget* TalentWidget = Cast<UTalentWidget>(child))
            {
                TalentWidget->OwningWidget = this;
            }
        }
    }
}

void UTalentTreeWidget::CheckLearnedAbilities()
{
    for (auto* Widget : StrengthRows)
    {
        if (!Widget) continue;
        for (auto* child : Widget->GetAllChildren())
        {
            if (UTalentWidget* TalentWidget = Cast<UTalentWidget>(child))
            {
                if(TalentWidget->bIsAbilityLearned())
                {
                    TalentWidget->SetIsEnabled(false);
                    TalentWidget->SetRenderOpacity(0.8f);
                }
                else
                {
                    TalentWidget->SetIsEnabled(true);
                    TalentWidget->SetRenderOpacity(1.f);
                }
            }
        }
    }
    for (auto* Widget : IntellectRows)
    {
        if (!Widget) continue;
        for (auto* child : Widget->GetAllChildren())
        {
            if (UTalentWidget* TalentWidget = Cast<UTalentWidget>(child))
            {
                if (TalentWidget->bIsAbilityLearned())
                {
                    TalentWidget->SetIsEnabled(false);
                    TalentWidget->SetRenderOpacity(0.5f);
                }
                else
                {
                    TalentWidget->SetIsEnabled(true);
                    TalentWidget->SetRenderOpacity(1.f);
                }
            }
        }
    }
}

