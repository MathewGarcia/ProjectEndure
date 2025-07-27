// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffSelectionWidget.h"
#include "Buff.h"
#include "BuffDataAsset.h"
#include "Buff_Health.h"
#include "GameStats.h"
#include "Components/TileView.h"

void UBuffSelectionWidget::Init(const TArray<UBuffDataAsset*>& SelectedBuffs, const TArray<UBuffDataAsset*>& EveryBuff)
{
    ChosenBuffs = SelectedBuffs;
    AllBuffs = EveryBuff;
}

void UBuffSelectionWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!BuffSlotTV) return;
    for (int i = 0; i < ChosenBuffs.Num(); ++i)
    {
        if (ChosenBuffs[i]) {
            UBuffDataAsset* Duplicate = DuplicateObject(ChosenBuffs[i], this);
            if (Duplicate)
            {
                BuffSlotTV->AddItem(Duplicate);
            }
        }
    }
    BuffSlotTV->OnItemClicked().AddLambda([this](UObject* Item)
        {
            if (UBuffDataAsset* SelectedDataObject = Cast<UBuffDataAsset>(Item))
            {
                if (SelectedDataObject)
                {
                    UE_LOG(LogTemp, Warning, TEXT("%s Item was clicked"), *SelectedDataObject->BuffName.ToString());
                    APawn* PlayerPawn = GetOwningPlayerPawn();
                    if (!PlayerPawn) return;
                    if(SelectedDataObject->BuffObjectToSpawn)
                    {
                        if(UBuff*SpawnedBuffObject = NewObject<UBuff>(PlayerPawn,SelectedDataObject->BuffObjectToSpawn))
                        {
                            if(UBuff_Health*BuffStat = Cast<UBuff_Health>(SpawnedBuffObject))
                            {
                                switch (SelectedDataObject->BuffIndex)
                                {
                                case 1:
                                    BuffStat->StatType = EPlayerStatType::Health;
                                    break;
                                case 2:
                                    BuffStat->StatType = EPlayerStatType::Stamina;
                                    break;
                                case 3:
                                    BuffStat->StatType = EPlayerStatType::Intellect;
                                    break;
                                case 4:
                                    BuffStat->StatType = EPlayerStatType::Strength;
                                    break;
                                case 5:
                                    BuffStat->StatType = EPlayerStatType::Life;
                                    break;
                                default:
                                    BuffStat->StatType = EPlayerStatType::Health;
                                    break;
                                }
                            }
                            SpawnedBuffObject->Execute();
                            OnBuffSelected.Broadcast();
                            BuffSlotTV->SetIsEnabled(false);
                            SetVisibility(ESlateVisibility::Hidden);
                        }
                    }
                }
            }
        });
}
