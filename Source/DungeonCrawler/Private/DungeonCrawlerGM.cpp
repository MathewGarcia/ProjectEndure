// Fill out your copyright notice in the Description page of Project Settings.

#include "DungeonCrawlerGM.h"
#include "MainGameInstance.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"
#include "PlayerSaveGame.h"

void ADungeonCrawlerGM::IncrementRoundCount(int NewRoundCount)
{
    roundCount += NewRoundCount;
    OnRoundEndDelegate.Broadcast();
}

void ADungeonCrawlerGM::SetRoundCount(int NewRoundCount)
{
    roundCount = NewRoundCount;
}

void ADungeonCrawlerGM::InitializeRound()
{
    UWorld* World = GetWorld();
    if (!World) return;
    if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
    {
        MGI->LoadGameData();
        if(MGI->LoadType == ELoadType::GameInstance)
        {
            roundCount = MGI->GIRound;
        }
        else if (UPlayerSaveGame* Save = MGI->GetSavedGame())
        {
            if (MGI->bLoadingSucceeded && MGI->LoadType == ELoadType::SaveGame) {
                roundCount = Save->Round;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Couldn't get save"));
        }
    }
}

void ADungeonCrawlerGM::Loading()
{
    UWorld* World = GetWorld();
    if (!World) return;
    if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
    {
        if (APlayerCharacter* player = MGI->localPlayer)
        {
            TWeakObjectPtr<UMainPlayerWidget> MPW = player->GetMainPlayerWidget();
            if (MPW.IsValid())
            {
                if (bIsLoading) {
                    MPW->SetLoadVisibility(ESlateVisibility::Visible);
                }
                else
                {
                    MPW->SetLoadVisibility(ESlateVisibility::Collapsed);
                }
            }
        }
        World->GetTimerManager().ClearTimer(FRemoveFogUITimer);
    }
}

void ADungeonCrawlerGM::SetPlayer(APlayerCharacter* player)
{
    SafePlayer = player;
}

void ADungeonCrawlerGM::SetDungeonReady()
{
    bDungeonReady = true;
}

void ADungeonCrawlerGM::CheckReady()
{
    UWorld* World = GetWorld();
    if (!World) return;
    if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
        if (APlayerCharacter* player = MGI->localPlayer)
        {
            if (APlayerCharacterState* PCS = player->GetPlayerCharacterState())
            {
                if (PCS->bInDungeon)
                {
                    if (bDungeonReady)
                    {
                        bIsLoading = false;
                        Loading();
                    }
                }
                else
                {
                    bIsLoading = false;
                    Loading();
                }
            }
        }
}

void ADungeonCrawlerGM::InitializeTimer()
{
    UWorld* World = GetWorld();
    if (!World) return;
    World->GetTimerManager().SetTimer(FRemoveFogUITimer, this, &ADungeonCrawlerGM::CheckReady, 0.5, true);
}

void ADungeonCrawlerGM::BeginPlay()
{
    UWorld* World = GetWorld();
    if (!World) return;
    if(UMainGameInstance*MGI = Cast<UMainGameInstance>(World->GetGameInstance()))
    {
        MGI->OnDungeonGenerationComplete.AddDynamic(this, &ADungeonCrawlerGM::SetDungeonReady);
    }
    InitializeTimer();
    InitializeRound();
}
